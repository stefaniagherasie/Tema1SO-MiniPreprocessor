#include "directory.h"

#define ERROR_CODE 12
#define MAX_PATH_SIZE 40


/* Allocate space for the DirArray structure */
DirArray *initDirs(int size)
{
	DirArray *dir_arr = NULL;

	dir_arr = (DirArray *) malloc(sizeof(DirArray));
	if (!dir_arr) {
		free(dir_arr);
		return NULL;
	}

	dir_arr->dirs = (char **) calloc(sizeof(char *), size);
	if (!dir_arr->dirs) {
		free(dir_arr->dirs);
		return NULL;
	}

	dir_arr->dir_count = 0;

	return dir_arr;
}


/* Adds a directory to the structure */
int addDir(DirArray *dir_arr, char *dir_name)
{
	int cnt = dir_arr->dir_count;
	char **dirs = dir_arr->dirs;

	dirs[cnt] = (char *) calloc(sizeof(char), strlen(dir_name) + 1);
	if (!dirs[cnt]) {
		free(dirs[cnt]);
		freeDirs(dir_arr);
		return ERROR_CODE;
	}

	/* Copy the name of the directory into the structure */
	strcpy(dirs[cnt], dir_name);

	dir_arr->dir_count = dir_arr->dir_count + 1;

	return 1;
}


/*	Removes all of the directories and frees space */
void freeDirs(DirArray *dir_arr)
{
	int i;

	for (i = 0; i < dir_arr->dir_count; ++i) {
		if (dir_arr->dirs[i] != NULL)
			free(dir_arr->dirs[i]);
	}

	free(dir_arr->dirs);
	free(dir_arr);
}
