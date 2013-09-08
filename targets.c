// CMPSC 311, Project 8, emulate "make -n"

/*
	author: Nathan Bialas
	email: njb5174@psu.edu
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "targets.h"
#include "sources.h"
#include "recipes.h"


static struct target target_list = {
	.next = NULL,
  .target_name = NULL,
  .target_sources = NULL,
  .target_recipes = NULL,
  
};

void target_list_print(void)
{
	struct target *current = &target_list;
	
	if( current->next == NULL)
	{
		if(current->target_name != NULL) //First element
		{
			printf("target Name:  %s\n", current->target_name);
			printf("	Sources\n");
			source_list_print(current->target_sources);
			printf("	Recipe Lines:\n");
			recipe_list_print(current->target_recipes);
		}
		
		else
		{
			printf("No targets Found\n");
		}
		
	}
	
	else
	{
		//Display list -1 element
		while(current->next != NULL)
		{
			printf("target Name:  %s\n", current->target_name);
			printf("	Sources\n");
			source_list_print(current->target_sources);
			printf("	Recipe Lines:\n");
			recipe_list_print(current->target_recipes);

			current = current->next;			
		}
		
		//display last element
		printf("target Name:  %s\n", current->target_name);
		printf("	Sources\n");
		source_list_print(current->target_sources);
		printf("	Recipe Lines:\n");
		recipe_list_print(current->target_recipes);
	}
	
}

struct target* find_target(char* target_name)
{
	struct target *temp = &target_list;
	
	if(target_list.target_name == NULL)
	{
		printf("Target not found because there are no targets\n");
	}
	else
	{
		if(temp->next == NULL) //first element
		{
			if(strcmp(temp->target_name, target_name) == 0)
			{
				return temp;
			}
			else
			{
				return NULL;
			}
		}
		
		else
		{
			while(temp->next != NULL)
			{
				if(strcmp(temp->target_name, target_name) == 0)
				{
					return temp;
				}
				else
				{
					temp = temp->next;
				}
			}
			

			if(strcmp(temp->target_name, target_name) == 0)
				{
					return temp;
				}
			else
			{
				return NULL;
			}
		}
	}
	
	return NULL; //If this is reached something confusing happened
}

void add_target(char* name, struct source* sources, struct recipe* recipes)
{
	struct target *temp = &target_list;

	
		if(target_list.target_name == NULL) //List is empty
		{
			target_list.target_name = name;
			target_list.target_sources = sources;
			target_list.target_recipes = recipes;
			return;
		}
		
		struct target* new_target = malloc(sizeof(struct target));
		new_target->target_name = name;
		new_target->target_sources = sources;
		new_target->target_recipes = recipes;
		new_target->next = NULL;
		
		int count_list = 0;
			
		//new_target->target_length = count;
		
		while( temp->next != NULL)
		{
			count_list++;
			temp = temp->next;
		}
		
		if(count_list == 0)
		{
			target_list.next = new_target;
		}
		
		else
		{
			temp->next = new_target;
		}
		
	
}


