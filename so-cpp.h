#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "directory.h"
#include "hashmap.h"
#include "utils.h"

char *input_name;
char *output_name;
FILE *in_file;
FILE *out_file;

HashMap *hashmap;
DirArray *dir_arr;

/* Adds the entry (key, value) from the command line parameters */
int addMappingFromParam(char *argument);

/* Parse the directives given in the command line */
int parseParams(int argc, char **argv);

/* Adds the entry (key, value) from the file */
int addMappingFromFile(char *argument);

/* Replaces one apperance of the key with the mapping */
char *replaceDefine(char *line, char *key, char *value);

/* Replaces all apperances of any key in the line */
char *applyDefine(char *str);

/* Replaces all apperances of any key that is not between commas */
char *applyDefineWithCommas(char *line);

/* Test if the condition is true, according to type */
/* type 0 - if/elif,	type 1 - ifdef,	  type 2 - ifndef */
int testIfCondition(char *line, int type);

/* Get the name of the initial directory from inpur file name*/
char *getInitialDir(char *input_name);

/* Parse the input file, preprocessing the content */
int parseContent(FILE *current_input);
