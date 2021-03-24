#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"


typedef struct DirArray {
	char **dirs;
	int dir_count;
} DirArray;

/* Allocates space for the DirArray structure */
DirArray *initDirs(int size);

/* Adds a directory to the structure */
int addDir(DirArray *dir_arr, char *dir_name);

/* Removes all of the directories and frees space */
void freeDirs(DirArray *dir_arr);
