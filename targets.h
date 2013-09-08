/* CMPSC 311, Spring 2013, Project 8
 *
 
 	written by Nathan Bialas
 	email: njb5174@psu.edu
 */

struct target {
  char* target_name;
  
  struct source *target_sources;
  struct recipe *target_recipes;
  struct target *next;
};


void  target_list_print(void);
struct target* find_target(char* target_name);
void add_target(char* name, struct source* sources, struct recipe* recipes);
