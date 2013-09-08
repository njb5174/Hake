// CMPSC 311, Project 8, emulate "make -n"

/*
	author: Nathan Bialas
	email: njb5174@psu.edu




 sketch of a possible solution
 * 1. read the command line
 *    options -h -v -f something
 *    which file should we read?
 *      hakefile, Hakefile, or something
 *      makefile, Makefile, or something
 *    what are the goals?
 *      argv[] elements after the last option
 *      default goal, see step 2
 * 2. read the file
 *    determine the default goal
 *      the first target mentioned in the file
 *    version 1: list of rules, direct from the input
 *      for each rule, 
 *        target-dependency line, file name and line number
 *        list of targets
 *        list of sources
 *        list of recipes
 *    version 2: list of targets, easier to search
 *      for each target,
 *        target-dependency line, file name and line number
 *        list of sources
 *        list of recipes
 * 3. extract appropriate recipes from the goals and the lists
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

// ... more later
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>

#include "macro.h"
#include "recipes.h"
#include "sources.h"
#include "targets.h"
#include "cmpsc311.h"
#include "names.h"

int  pr8_read(char *file);	// return 1 if successful, 0 if not
void pr8_work(char *goal);	// recursive


static void usage(int status)
{
  if (status == EXIT_SUCCESS)
    {
      printf("usage: [-h] [-v] [-f file]\n");
      printf("  -h           print help\n");
      printf("  -v           verbose mode; enable extra printing; can be repeated\n");
      printf("  -f file      input filename; default is hakefile or Hakefile\n");
    }
  else
    {
      fprintf(stderr, ": Try ' -h' for usage information.\n");
    }

  exit(status);
}

// last-modification time of a file

// The appropriate field of struct stat varies according to the OS.
//   Posix, 2001        time_t st_mtime
//   Posix, 2004        time_t st_mtime
//   Posix, 2008        struct timespec st_mtim
//                      macro st_mtime is st_mtim.tv_sec
//   Solaris            time_t st_mtime
//   Linux              time_t st_mtime
//   Mac OS X           struct timespec st_mtimespec
//                      or, st_mtime

// The struct timespec fields are defined in <time.h>.
//   time_t tv_sec;     // seconds
//   long   tv_nsec;    // nanoseconds

// The type time_t (time in seconds) is defined in <time.h> and <sys/types.h>.
// The Posix Standard allows time_t to be an integer, or a real-floating type.
// If time_t isn't an integer, we would be surprised, but the code should
//   still work properly.

// A nonexistent file is treated as extremely old.

static struct timespec mtime(const char *file)
{
  struct stat s;
  struct timespec t = { 0, 0 };

  if (stat(file, &s) == 0)
#if   defined(MTIME) && MTIME == 1      // Linux
    { t = s.st_mtim; }
#elif defined(MTIME) && MTIME == 2      // Mac OS X
    { t = s.st_mtimespec; }
#elif defined(MTIME) && MTIME == 3      // Mac OS X, with some additional settings
    { t.tv_sec = s.st_mtime; t.tv_nsec = s.st_mtimensec; }
#else                                   // Solaris
    { t.tv_sec = s.st_mtime; }
#endif

  return t;
}

static void read_lines(char *filename, FILE *fp);

//Sets on char string to another
static void set_chars(char *first, char *second)
{
	int count = 0;
	while(*(second+count) != '\0')
	{
		*(first+count) = *(second+count);
		count++;
	}
	
	*(first+count) = '\0';
	
}


// ... more later

// global data, accumulated from reading the file
char *default_goal = NULL;	// first-mentioned target
// ... lists
char *first_related_target = NULL; //name of first target in a line, both share sources/recipes
int target_length = 0;  //name of last target in a line, both share sources/recipes


#define MAXLINE 4096

int main(int argc, char *argv[])
{
  // command-line arguments
  
      
  extern char *optarg;
  extern int optind;
  extern int optopt;
  extern int opterr;
  int verbose = 0;
  int f_flag = 0;
  int ch;
  
   while ((ch = getopt(argc, argv, "hvf::")) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case 'f': /* interactive mode */
                
                break;
            case 'v':
                verbose = 1;
                break;
            case '?':
                printf("%s: invalid option '%c'\n", argv[0], optopt);
                usage(EXIT_FAILURE);
                break;
            case ':':
                printf("%s: invalid option '%c' (missing argument)\n", argv[0], optopt);
                usage(EXIT_FAILURE);
                break;
            default:
                usage(EXIT_FAILURE);
                break;
        }
    }
    
      // scan the argv array again, from the beginning
  optind = 1;
  while ((ch = getopt(argc, argv, ":hvf:")) != -1)
    {
      switch (ch) {
	case 'f':
	  f_flag++;		// number of -f options supplied
	  (void) pr8_read(optarg);
	  break;
        default:
          break;
      }
    }


  // read the file
    if (f_flag == 0 && !pr8_read("hakefile") && !pr8_read("Hakefile"))
    {
      fprintf(stderr, "%s: no input\n", prog);
      usage(EXIT_FAILURE);
    }
  

  // evaluate the goals
  if (optind == argc)
    { pr8_work(default_goal); }
  else
    {
      for (int i = optind; i < argc; i++)
        { pr8_work(argv[i]); }
    }

  return 0;
}

int pr8_read(char *file)
{
  if (file == NULL) return 0;

  printf("file: %s\n", file);
  
  static struct list_names * filenames = NULL;
  if (filenames == NULL) filenames = list_names_allocate("filenames");
    // by construction, filenames is now not NULL
    
    // if (file is on the list already) { return 1 }
  // else { put filename on the list and continue }
  if (list_names_append_if_new(filenames, file) == 1)
    { return 1; }
    
    if (strcmp(file, "-") == 0)
    { read_lines("[stdin]", stdin); return 1; }
    
    FILE *fp = fopen(file, "r");
    
    if (fp == NULL)
    {
//		fprintf(stderr, "%s: could not open input file %s: %s\n", prog, file, strerror(errno));
      return 0;
    }
    
    read_lines(file, fp);
    
    if (fclose(fp) != 0)
    {
      fprintf(stderr, "%s: could not close input file %s: %s\n", prog, file, strerror(errno));
    }

  return 1;

}

void pr8_work(char *goal)
{
  if (goal == NULL) return;

  printf("goal: %s\n", goal);
  
  struct target *target_goal = find_target(goal);
  
  if(target_goal == NULL)
  {
  	
  }
  
  else
  {
  	struct timespec t1 = mtime(target_goal->target_name);
  	
  	struct source *temp = target_goal->target_sources;
  	
  	bool recipe_flag = false;
  	  	
  	while(temp->next != NULL)
  	{
		struct timespec t2 = mtime(temp->source_name);  // source
		
		if (t1.tv_sec == 0  || t1.tv_sec < t2.tv_sec || ((t1.tv_sec == t2.tv_sec) && (t1.tv_nsec < t2.tv_nsec)))
		  { 
		  	recipe_flag = true;
		  
		   }
  		temp = temp->next;
  	}
  	
  	if(recipe_flag)
  	{
  		printf("%s is outdated or nonexistant, execute recipes\n", temp->source_name);
		  	recipe_list_print(target_goal->target_recipes);
		}
  }

  // if goal is a known target,
  //   iterate through goal's list of sources,
  //     call pr8_work() on each source
  //   iterate through goal's list of sources,
  //     compare times of goal and its sources
  //   if necessary, iterate through goal's list of recipes,
  //     print the recipe

  // ... more later

  return;
}

static void read_lines(char *filename, FILE *fp)
{
  

 // if (verbose > 0)
   // { fprintf(stderr, "%s: read_lines(%s)\n", prog, filename); }

  char original[MAXLINE+2];	// from fgets()
  char expanded[MAXLINE+2];	// after macro expansion
  char buffer[MAXLINE+2];	// working copy, safe to modify

  char whsp[] = " \t\n\v\f\r";			// whitespace characters
  int line_number = 0;
  int recipe_line_number = 0;

  bool have_target = false;			// recipes must follow targets

  while (fgets(original, MAXLINE, fp) != NULL) {
    // it is possible that the input line was too long, so terminate the string cleanly
    original[MAXLINE] = '\n';
    original[MAXLINE+1] = '\0';

    line_number++;
   if (verbose > 0) printf("%s: %s: line %d: %s\n", "prog", filename, line_number, original);

    // assume original[] is constructed properly
    // assume expanded[] is large enough
    macro_expand(original, expanded);
   if (verbose > 0) printf("%s: %s: line %d: %s\n", "prog", filename, line_number, expanded);

    strcpy(buffer, expanded);			// copy, safe to modify

    char *buf = buffer;

    while (*buf == ' ') buf++;			// skip past leading spaces (not tabs!)

    char *p_hash = strchr(buf, '#');		// a comment starts with #
    if (p_hash != NULL)
      { *p_hash = '\0'; }			// remove the comment

    int n = 0;					// remove trailing whitespace
    while (buf[n] != '\0')
      {
        int n1 = strspn(&buf[n], whsp);		// buf[n .. n+n1-1] is whitespace
        int n2 = strcspn(&buf[n + n1], whsp);	// buf[n+n1 .. n+n1+n2-1] is not
        if (n2 == 0) { buf[n] = '\0'; break; }	// remove trailing whitespace
        n += n1 + n2;
      }

    if (buf[0] == '\0')				// nothing left?
      { continue; }

    char *p_colon = strchr(buf, ':');		// : indicates a target-prerequisite line
    char *p_equal = strchr(buf, '=');		// = indicates a macro definition

    if (buffer[0] == '\t')
      {
		recipe_line_number++;
			if (verbose > 0) printf("  diagnosis: recipe line %d\n", recipe_line_number);
		if (have_target == false)
		  {
			fprintf(stderr, "%s: %s: line %d: recipe but no target\n", prog, filename, line_number);
			continue;
		  }

		int count = 0;
				while(*(buffer+1+count) != '\0')
				{
					count++;
				}
				
			char *recipe_temp = malloc((count+1)*sizeof(char));
			set_chars(recipe_temp, buffer+1);
				
		struct target* temp_update_target = find_target(first_related_target);
		for(int i = 0; i < target_length; i++)			
		{
				temp_update_target->target_recipes = add_recipe(temp_update_target->target_recipes, recipe_temp);
				if(target_length > 1)
					temp_update_target = temp_update_target->next;
		}
		

		
		
		
      }
    else if (p_colon != NULL)
      {
     	 first_related_target = NULL;
		target_length = 0;
	recipe_line_number = 0;
        if (verbose > 0) printf("  diagnosis: target-prerequisite\n");
	have_target = true;
	
	char *source_start = p_colon+1;
	
	while(*source_start == ' ')
	{	
		source_start++;
	}
	
	char *source_end = source_start;
	
	struct source *source_head = malloc(sizeof(struct source));
		
	if(source_head == NULL)
	{
		printf("Malloc failed to allocate space for a source_head in the main program\n");
		return;
	}
	
	if(*source_end == '\n' || *source_end == '\0' || *source_end == '\r')
	{
		source_head = add_source(source_head, "");
	}
	
	while(*source_end != '\n' && *source_end != '\0' && *source_end != '\r')
	{
		if(*source_end == ' ' || *source_end == '\0')
		{
			
			*source_end = '\0';
			
			int count = 0;
				while(*(source_start+count) != '\0')
				{
					count++;
				}
				
			char *source_temp = malloc((count+1)*sizeof(char));
			set_chars(source_temp, source_start);
			source_head = add_source(source_head, source_temp);
			
			source_start = source_end+1;
		}
		
		if(*(source_end+1) == '\0')
		{
			int count = 0;
				while(*(source_start+count) != '\0')
				{
					count++;
				}
				
			char *source_temp = malloc((count+1)*sizeof(char));
			set_chars(source_temp, source_start);
			source_head = add_source(source_head, source_temp);
			
		}
		
		source_end++;

	}

	char *tar_start = buf;
	while(*tar_start == ' ')
	{
		tar_start++;
	}
	
	char *tar_end = tar_start;
	
	
	while(*tar_end != ':' && *tar_end != '\0')
	{	
		if(*tar_end == ' ' || *tar_end == '\t')
		{		

			*tar_end = '\0';
			if(default_goal == NULL)
			{
				int count = 0;
				while(*(tar_start+count) != '\0')
				{
					count++;
				}
				
				default_goal = malloc((count+1)*sizeof(char));
				set_chars(default_goal, tar_start);
			}
			
			struct recipe* super_temp = malloc(sizeof(struct recipe));
			super_temp->line = "replaceme";
			
			char* temp = malloc(30 * sizeof(char));
			set_chars(temp, tar_start);
			
			add_target(temp, source_head, super_temp);

			
			if(first_related_target == NULL)
			{
				int count = 0;
				while(*(tar_start+count) != '\0')
				{
					count++;
				}
				
				first_related_target = malloc((count+1)*sizeof(char));
				if (first_related_target == NULL)
				{
					printf("malloc failed\n");
				}
				
				for(int i = 0; i < count+2; i++)
				{
					*(first_related_target+i) = *(tar_start+i);
				}
				
			}
			
			target_length++;
			tar_start = (tar_end+1);
		}
		
		if(*(tar_end+1) == ':' )
		{
			if(*(tar_end) != ' ')
			{
				int count = 0;
					while(*(tar_start+count) != ':')
					{
						count++;
					}
					
				*(tar_end+1) = '\0';
					
				char *tar_temp = malloc((count+1)*sizeof(char));
				set_chars(tar_temp, tar_start);
				struct recipe* super_temp = malloc(sizeof(struct recipe));
				super_temp->line = "replaceme";
				
				
				add_target(tar_temp, source_head, super_temp);
				
				if(first_related_target == NULL)
			{
				int count = 0;
				while(*(tar_start+count) != '\0')
				{
					count++;
				}
				
				first_related_target = malloc((count+1)*sizeof(char));
				if (first_related_target == NULL)
				{
					printf("malloc failed\n");
				}
				
				set_chars(first_related_target, tar_start);
				
			}
			}
			
		}
		
		tar_end++;

	}	
		
	
	}
	
    else if (p_equal != NULL)
      {
      	first_related_target = NULL;
		target_length = 0;
		
        if (verbose > 0) printf("  diagnosis: macro definition\n");
	have_target = false;
        // name = body
        // *p_equal is '='
        char *name_start = buf;
        while (*name_start == ' ' || *name_start == '\t')       // skip past spaces and tabs
          { name_start++; }
        char *name_end = p_equal-1;
        while (*name_end == ' ' || *name_end == '\t')
          { name_end--; }
        name_end++;
        *name_end = '\0';
        char *body_start = p_equal+1;
        while (*body_start == ' ' || *body_start == '\t')
          { body_start++; }
        char *body_end = body_start;
        while (*body_end != '\0')       // end of string
          { body_end++; }
        while (*body_end == ' ' || *body_end == '\t')
          { body_end--; }
        body_end++;
        *body_end = '\0';
        if (verbose > 1) macro_list_print();
        macro_set(name_start, body_start);
        if (verbose > 1) macro_list_print();
      }
    else if (strncmp("include", buf, 7) == 0)
      {
      	first_related_target = NULL;
		target_length = 0;
        if (verbose > 0) printf("  diagnosis: include\n");
	have_target = false;
	char *name_start = buf + 7;				// skip past "include"
	while (*name_start == ' ' || *name_start == '\t')	// skip past spaces and tabs
	  { name_start++; }
	if (*name_start == '\0')
	  {
	    // following GNU Make, this is not an error
	    if (verbose > 0) fprintf(stderr, "%s: %s: line %d: include but no filename\n", prog, filename, line_number);
	    continue;
	  }
	else if (*name_start == '\'' || *name_start == '"')		// quoted filename
	  {
	    // find matching quote, remove it
	    char *q = name_start + 1;				// skip past ' or "
	    while (*q != *name_start && *q != '\0') q++;	// find end of string or line
	    if (*q == '\0')
	      {
		fprintf(stderr, "%s: %s: line %d: file name error [%s]\n", prog, filename, line_number, name_start);
		continue;
	      }
	    name_start++;	// skip past opening quote
	    *q = '\0';		// remove closing quote
	  }
        pr8_read(name_start);
      }
    else
      {
        if (verbose > 0) printf("  diagnosis: something else\n");
	have_target = false;
//	fprintf(stderr, "%s: %s: line %d: not recognized: %s", prog, filename, line_number, original);
      }
  }

//  if (ferror(fp))	// error when reading the file
  //  { fprintf(stderr, "%s: %s: read error: %s\n", prog, filename, strerror(errno)); }

  return;
}

