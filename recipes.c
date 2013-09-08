// CMPSC 311, Project 8, emulate "make -n"

/*
	author: Nathan Bialas
	email: njb5174@psu.edu
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recipes.h"



void  recipe_list_print(struct recipe* list)
{
	struct recipe *current = list;
	
	if( current->next == NULL)
	{
		if(current->line != NULL) //First element
		{
			printf("Line:  %s\n", current->line);
		}
		
		else
		{
			printf("No Recipes Found");
		}
		
	}
	
	else
	{
		//Display list -1 element
		while(current->next != NULL)
		{
			printf("Line:  %s\n", current->line);

			current = current->next;			
		}
		
		//display last element
		printf("Line:  %s\n", current->line);
	}
	
}

struct recipe* add_recipe(struct recipe* list, char* new_line)
{
	if(strcmp(list->line, "replaceme") == 0) //List is empty
	{
		int count = 0;
		while(*(new_line+count) != '\0')
			count++;
		
		count++;
		
		list->line = new_line;
		list->line_length = count;
		list->next = NULL;
				
		return list;
	}

	struct recipe *temp = list;
	struct recipe *new_recipe = malloc(sizeof(struct recipe));
	
	if(new_recipe == NULL)
	{
		printf("malloc failed to allocate space for a new recipe\n");
	}
	else
	{
		

		int count_list = 0;
		new_recipe->next = NULL;
		new_recipe->line = new_line;
		int count = 0;
		
		while(*(new_line+count) != '\0')
			count++;
			
		count++;
			
		new_recipe->line_length = count;
		
		while( temp->next != NULL)
		{
			count_list++;
			temp = temp->next;
		}
		
		if(count_list == 0)
		{
			list->next = new_recipe;
			return list;
			
		}
		
		else //add new_recipe to the end of the list
		{
			temp->next = new_recipe;
			return list;
		}
		
	}

		return NULL; //If this is reached something confusing happened

}
