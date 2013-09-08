/* CMPSC 311, Spring 2013, Project 8
 *
 
 	written by Nathan Bialas
 	email: njb5174@psu.edu
 */

struct recipe {
  char* line;
  int line_length;
  struct recipe *next;
};

void  recipe_list_print(struct recipe* list);

//Returns the head of the new list
struct recipe*  add_recipe(struct recipe* list, char* new_recipe);