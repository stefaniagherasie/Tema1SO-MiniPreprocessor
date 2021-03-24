#include "so-cpp.h"
#define BUFSIZE 256
#define ERROR_CODE 12
#define SIZE 10

/* Adds the entry (key, value) from the command line parameters */
int addMappingFromParam(char *argument)
{
	int r = 0;
	char *key; char *value; char *ptr;
		
	ptr = strchr(argument, '=');
	if (ptr == NULL) {
		/* The mapping is missing, it is considered the empty string */
		key = argument;
		value = "";
	} else {
		key = strtok(argument, "=");
		value = strtok(NULL, "=");
	}

	/* Add entry in the HashMap structure */
	r = putPair(hashmap, key, value);
	if (r == ERROR_CODE)
		return ERROR_CODE;

	return 1;
}


/* Parse the directives given in the command line */
int parseParams(int argc, char **argv)
{
	/* Count the number of input/outpur file for later checks */
	int i, r, out_count = 0, in_count = 0;

	for (i = 1; i < argc; ++i) {
		/* -D directive */
		if (!strncmp(argv[i], "-D", 2)) {
			/* -D <SYMBOL>[=<MAPPING>] or -D<SYMBOL>[=<MAPPING>] */
			if (!strcmp(argv[i], "-D"))
				i++;
			else
				memmove(argv[i], argv[i] + 2, strlen(argv[i]));

			r = addMappingFromParam(argv[i]);
			if (r == ERROR_CODE)
				return ERROR_CODE;
		}

		/* -I directive */
		else if (!strncmp(argv[i], "-I", 2)) {
			/* -I <DIR> or -I<DIR> */
			if (!strcmp(argv[i], "-I"))
				i++;
			else
				memmove(argv[i], argv[i] + 2, strlen(argv[i]));

			r = addDir(dir_arr, argv[i]);
			if (r == ERROR_CODE)
				return ERROR_CODE;
		}

		/* Output file (-o directive) */
		else if (!strncmp(argv[i], "-o", 2)) {
			/* -o <OUTFILE> or -o<OUTFILE> */
			if (!strcmp(argv[i], "-o"))
				i++;
			else
				memmove(argv[i], argv[i] + 2, strlen(argv[i]));

			output_name = argv[i];
			out_count++;
		}

		/* Input file (or Output file without -o) */
		else {
			if (input_name == NULL) {
				input_name = argv[i];
				in_count++;
			} else {
				output_name = argv[i];
				out_count++;
			}
		}
	}

	/* If there are multiple input/output files, bad parameters */
	if (in_count > 1 || out_count > 1)
		return -1;

	return 1;
}


/* Adds the entry (key, value) from the file */
int addMappingFromFile(char *argument)
{
	int r;
	char *token; char *key;
	char *value = (char *) calloc(sizeof(char), strlen(argument) + 1);
	char value2[BUFSIZE];

	/* Erase #define from argument */
	memmove(argument, argument + strlen("#define "), strlen(argument));

	/* Get the key and obtain the mapping through concatenation */
	/* The input string may have "\"" in it from multiple line DEFINE */

	key = strtok(argument, " \\\n");
	token = strtok(NULL, " \\\n");
	while (token != NULL) {
		strcat(value, token);
		strcat(value, " ");
		token = strtok(NULL, " \\\n");
	}

	/* Erase the last trailing space */
	strcpy(value2, value);
	free(value);
	value2[strlen(value2) - 1] = '\0';

	/* Add entry in the HashMap structure */
	r = putPair(hashmap, key, value2);
	if (r == ERROR_CODE)
		return ERROR_CODE;

	return 1;
}


/* Replaces one apperance of the key with the mapping */
char *replaceDefine(char *line, char *key, char *value)
{
	char end[BUFSIZE]; char *ptr; char *tmp;

	tmp = (char *) calloc(sizeof(char), BUFSIZE);
	if (tmp == NULL)
		exit(ERROR_CODE);

	ptr = strstr(line, key);
	if (ptr != NULL) {
		/* Erase the key and the front part of the string */
		memmove(ptr, ptr + strlen(key), strlen(ptr));
		strcpy(end, ptr);

		/* Erase the key and the end part of the string */
		line[strlen(line) - strlen(ptr)] = '\0';

		/* Form the string with the new mapping */
		strcpy(tmp, line);
		strcat(tmp, value);
		strcat(tmp, end);

		return tmp;
	}

	free(tmp);
	return NULL;
}


/* Replaces all apperances of any key in the line */
char *applyDefine(char *str)
{
	int i;
	char *ptr = NULL;
	char *key; char *value; char *line;

	line = (char *) calloc(sizeof(char), BUFSIZE);
	if (line == NULL) {
		free(line);
		exit(ERROR_CODE);
	}
	strcpy(line, str);

	for (i = 0; i < hashmap->size; ++i) {
		if (hashmap->list[i] != NULL) {
			/* Get each key and test if the line contains it */
			key = hashmap->list[i]->key;
			value = getValue(hashmap, key);

			ptr = strstr(line, key);
			while (ptr != NULL) {
				char *tmp;

				/* Replace each appearance of the key */
				tmp = replaceDefine(line, key, value);
				if (tmp != NULL) {
					memset(line, 0, strlen(line));
					strcpy(line, tmp);
					free(tmp);
				}

				ptr = strstr(line, key);
			}
		}
	}

	return line;
}


/* Replaces all apperances of any key that is not between commas */
char *applyDefineWithCommas(char *line)
{
	int comma = 0, i, len = strlen(line);
	char *defined_end; char *result;
	char start[BUFSIZE], end[BUFSIZE];

	strcpy(start, line);
	strcpy(end, line);

	for (i = 0; i < len; i++) {
		if (line[i] == '\"' && !comma)
			comma++;

		/* When finding the closing comma, save the start and end */
		else if (line[i] == '\"' && comma) {
			start[i + 1] = '\0';
			memmove(end, end + i + 1, strlen(end));
			break;
		}
	}

	/* Apply define on the end part */
	defined_end = applyDefine(end);

	len = strlen(start) + strlen(defined_end) + 1;
	result = (char *) calloc(sizeof(char), len);
	if (!result) {
		free(result);
		exit(ERROR_CODE);
	}

	/* Concatenate the start and end to obtain the final string */
	strcpy(result, start);
	strcat(result, defined_end);
	free(defined_end);

	return result;
}


/* Test if the condition is true, according to type */
/* type 0 - if/elif,	type 1 - ifdef,	  type 2 - ifndef */
int testIfCondition(char *line, int type)
{
	char *cond;

	/* Get the condition from the input line */
	cond = strtok(line, "\n ");
	cond = strtok(NULL, "\n ");

	/* #if / #elif */
	if (type == 1) {
		/* Replace the condition if it is a key in the map */
		if (containsKey(hashmap, cond))
			cond = getValue(hashmap, cond);

		/* Test if condition != 0, condition != FALSE */
		if (!(!strcmp(cond, "0") || !strcmp(cond, "FALSE")))
			return 1;
	}

	/* #ifdef */
	else if (type == 2) {
		if (containsKey(hashmap, cond))
			return 1;
	}

	/* #ifndef */
	else if (type == 3) {
		if (!containsKey(hashmap, cond))
			return 1;
	}

	return 0;
}


/* Get the name of the initial directory from input file name */
char *getInitialDir(char *input_name)
{
	char *ptr; char *dir_name;

	dir_name = (char *) calloc(sizeof(char), strlen(input_name) + 1);
	if (!dir_name) {
		free(dir_name);
		return NULL;
	}

	/* The directory is the current one, if we read from stdin */
	if (input_name == NULL)
		strcpy(dir_name, "./");

	/* The header is in the same directory as the input file */
	else {
		strcpy(dir_name, input_name);
		ptr = strrchr(input_name, '/');
		dir_name[strlen(input_name) - strlen(ptr)] = '\0';
		strcat(dir_name, "/");
	}

	return dir_name;
}


/* Parse the input file, preprocessing the content */
int parseContent(FILE *current_input)
{
	FILE *header_file;
	int test, i, r = 0, multiple_define = 0, can_write = 1, solved_if = 0;
	char path[100], buffer[BUFSIZE], define_buf[BUFSIZE];
	char *ptr;

	memset(buffer, 0, BUFSIZE);
	memset(define_buf, 0, BUFSIZE);

	/* Read each lone from the input_file */
	while (fgets(buffer, BUFSIZE, current_input) != NULL) {

		/* #define <SYMBOL> <MAPPING> */
		if (!strncmp(buffer, "#define", strlen("#define"))) {
			char *ptr = strstr(buffer, "\\\n");

			/* Check if line ends in / (multiple define) */
			if (ptr != NULL) {
				/* Mark the multiple-line define */
				strcat(define_buf, buffer);
				multiple_define = 1;
			} else {
				/* Add the normal entry into the HashMap */
				r = addMappingFromFile(buffer);
				if (r == ERROR_CODE)
					return ERROR_CODE;
			}
		}

		/* Concatenate lines if there is a multiple define */
		else if (multiple_define) {
			strcat(define_buf, buffer);

			/* Check for the last line in multiple define */
			ptr = strstr(buffer, "\\\n");
			if (ptr == NULL) {
				/* Add the entry into the HashMap */
				r = addMappingFromFile(define_buf);
				if (r == ERROR_CODE)
					return ERROR_CODE;

				/* Reset the variables used */
				multiple_define = 0;
				memset(define_buf, 0, BUFSIZE);
			}
		}

		/* #undef <SYMBOL> */
		else if (!strncmp(buffer, "#undef ", strlen("#undef "))) {
			char *key;

			key = strtok(buffer, "\n ");
			key = strtok(NULL, "\n ");
			/* Remove the pair(key, value) from HashMap */
			removeEntry(hashmap, key);
		}

		/* #if <CONDITION> / elif <CONDITION> */
		else if (!strncmp(buffer, "#if ", strlen("#if "))
			|| !strncmp(buffer, "#elif ", strlen("#elif "))) {

			/* The "if" clause was already true on a previous */
			/* branch, no need to test or print the block */
			can_write = 0;
			if (solved_if)
				continue;

			/* Test the condition and if true, mark that the */
			/* if was solved and the next block can be written */
			test = testIfCondition(buffer, 1);
			if (test) {
				solved_if = 1;
				can_write = 1;
			}
		}

		/* #ifdef <SYMBOL> */
		else if (!strncmp(buffer, "#ifdef ", strlen("#ifdef "))) {
			can_write = 0;

			/* Test the condition and if true, mark that the */
			/* if was solved and the next block can be written */
			test = testIfCondition(buffer, 2);
			if (test) {
				solved_if = 1;
				can_write = 1;
			}
		}

		/* #ifndef <SYMBOL> */
		else if (!strncmp(buffer, "#ifndef ", strlen("#ifndef "))) {
			can_write = 0;

			/* Test the condition and if true, mark that the */
			/* if was solved and the next block can be written */
			test = testIfCondition(buffer, 3);
			if (test) {
				solved_if = 1;
				can_write = 1;
			}
		}

		/* #else */
		else if (!strncmp(buffer, "#else", strlen("#else"))) {
			can_write = 0;

			/* Neither of the previous branches is true */
			if (!solved_if) {
				solved_if = 1;
				can_write = 1;
			}
		}

		/* #endif */
		else if (!strncmp(buffer, "#endif", strlen("#endif"))) {
			/* Reset the variables used for if clauses */
			solved_if = 0;
			can_write = 1;
		}

		/* #include <HEADER> */
		else if (!strncmp(buffer, "#include", strlen("#include"))) {
			char *current_dir; char *header;

			header = strtok(buffer, " \"\n");
			header = strtok(NULL, " \"\n");

			/* Get the path of the header from the current dir */
			current_dir = getInitialDir(input_name);
			strcpy(path, current_dir);
			strcat(path, header);
			free(current_dir);

			/* If the header exists in the current directory, */
			/* parse its content and read the next line */
			header_file = fopen(path, "r");
			if (header_file != NULL) {
				r = parseContent(header_file);
				if (r == -1 || r == ERROR_CODE)
					return r;
				continue;
			}

			/* Find the directory from command line */
			for (i = 0; i < dir_arr->dir_count; i++) {
				char *dir;

				memset(path, 0, strlen(path));
				dir = dir_arr->dirs[i];

				/* Form the path of the header */
				strcpy(path, dir);
				strcat(path, "/");
				strcat(path, header);

				/* Parse its content if it exists */
				header_file = fopen(path, "r");
				if (header_file != NULL) {
					r = parseContent(header_file);
					if (r == -1 || r == ERROR_CODE)
						return r;
					break;
				}
			}

			/* The header file does not exist */
			if (header_file == NULL)
				return -1;

		}

		/* Print if the line is part of the correct "if" block */
		/* or is a normal line which by default is written */
		else if (can_write) {
			char *tmp = NULL;

			/* Replace the keys with their mapping */
			if ((strchr(buffer, '\"')) != NULL)
				tmp = applyDefineWithCommas(buffer);
			else
				tmp = applyDefine(buffer);

			/* Print line in output_file/stdout */
			fputs(tmp, out_file);
			free(tmp);
		}

		memset(buffer, 0, BUFSIZE);
	}

	return 1;
}



int main(int argc, char **argv)
{
	int r = 0;
	/* Create a HashMap to hold the (key, value) pairs */
	hashmap = createMap(SIZE);
	if (hashmap == NULL)
		return ERROR_CODE;

	/* Create the structure to hold the directories */
	dir_arr = initDirs(argc / 2 + 1);
	if (dir_arr == NULL) {
		freeMap(hashmap);
		return ERROR_CODE;
	}


	/* Parse the command line arguments */
	r = parseParams(argc, argv);
	if (r == -1 || r == ERROR_CODE)
		return r;


	/* Open the input file or stdin if missing */
	if (input_name == NULL) {
		in_file = stdin;
	} else {
		in_file = fopen(input_name, "r");
		DIE(in_file == NULL, "Error at opening file");
	}

	/* Open the outputput file or stdout if missing */
	if (output_name == NULL) {
		out_file = stdout;
	} else {
		out_file = fopen(output_name, "w");
		DIE(out_file == NULL, "Error at opening file");
	}


	/* Parse the content by applying preprocessing directives */
	r = parseContent(in_file);
	if (r == -1 || r == ERROR_CODE)
		return r;


	/* Close the opened files */
	if (input_name != NULL)
		fclose(in_file);

	if (output_name != NULL)
		fclose(out_file);

	/* Free space */
	freeMap(hashmap);
	freeDirs(dir_arr);

	return 0;
}
