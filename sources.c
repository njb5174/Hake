// CMPSC 311, Project 8, emulate "make -n"

/*
	author: Nathan Bialas
	email: njb5174@psu.edu
	*/
	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sources.h"

void source_list_print(struct source* source_list)
{
	struct source *current = source_list;
	
	if( current->next == NULL)
	{
		if(current->source_name != NULL) //First element
		{
			printf("Source Name:  %s\n", current->source_name);
		}
		
		else
		{
			printf("No Sources Found\n");
		}
		
	}
	
	else
	{
		//Display list -1 element
		while(current->next != NULL)
		{
			printf("Source Name:  %s\n", current->source_name);

			current = current->next;			
		}
		
		//display last element
		printf("Source Name:  %s\n", current->source_name);
	}
	
}

struct source* add_source(struct source* list, char* name)
{
	if(list == NULL) //List is empty
	{
		int count = 0;
		while(*(name+count) != '\0')
			count++;
		
		count++;
		
		list->source_name = name;
		list->source_length = count;
		list->next = NULL;
		
		return list;
	}
	
	struct source *temp = list;
	struct source *new_source = malloc(sizeof(struct source));
	if(new_source == NULL)
	{
		printf("malloc failed to allocate space for a new source\n");
	}
	else
	{
		int count_list = 0;
		new_source->next = NULL;
		new_source->source_name = name;
		
		int count = 0;
		
		while(*(name+count) != '\0')
			count++;
		
		count++;
			
		new_source->source_length = count;
		
		while( temp->next != NULL)
		{
			count_list++;
			temp = temp->next;
		}
		
		if(count_list == 0)
		{
			if(list->source_name == NULL ) // first element
			{
				list = new_source;
				return list;
			}
		
			else // second element
			{
				list->next = new_source;
				return list;
			}
		}
		
		else //If the macro doesn't exist, its added to the end of the list and list is reset
		{
			temp->next = new_source;
			return list;
		}
		
	}
	
		return NULL; //If this is reached something confusing happened

}
