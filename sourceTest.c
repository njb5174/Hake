#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#include "sources.h"
#include "recipes.h"
#include "targets.h"


int main(int argc, char *argv[])
{
    
 	char *example = "example1";
 	char *second = "asdfasdf";
 	char *third = "lasty";
 	
 	struct source *source_list = malloc(sizeof(struct source));
 	
 	source_list = add_source(source_list, example); 	
 	source_list = add_source(source_list, second);
 	source_list = add_source(source_list, third);
 	 	
 	struct recipe *recipe_list = malloc(sizeof(struct recipe));
 	char *line1 = "This is recipe line1";
 	char *line2 = "gcc -Wall -Wextra";
 	char *line3 = "-v -f -h";
 	
 	recipe_list = add_recipe(recipe_list, line1);
 	recipe_list = add_recipe(recipe_list, line2);
 	recipe_list = add_recipe(recipe_list, line3);
 	 	
 	struct target *new_target = malloc(sizeof(struct target));
 	
 	new_target->target_name = "target1";
 	new_target->target_sources = source_list;
 	new_target->target_recipes = recipe_list;
 	new_target->next = NULL;
 	
 	add_target(new_target);
 	target_list_print();
 	

  
  
  exit(0);
}