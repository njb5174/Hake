/* CMPSC 311, Spring 2013, Project 8
 *
 
 	written by Nathan Bialas
 	email: njb5174@psu.edu
 */

struct source {
  char* source_name;
  int source_length;
  struct source *next;
};


void  source_list_print(struct source *source_list);

//Returns the head of the new list
struct source*  add_source(struct source* source_list, char* name);