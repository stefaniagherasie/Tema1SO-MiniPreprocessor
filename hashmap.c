#include "hashmap.h"
#define ERROR_CODE 12


/* Allocate space for the HashMap structure */
HashMap *createMap(int size)
{
	int i = 0;
	HashMap *map = NULL;

	map = (HashMap *) malloc(sizeof(HashMap));
	if (!map) {
		free(map);
		return NULL;
	}

	map->size = size;
	map->list = (Pair **) calloc(sizeof(Pair *), size);

	if (!map->list) {
		free(map->list);
		free(map);
		return NULL;
	}

	for (i = 0; i < size; i++)
		map->list[i] = NULL;

	return map;
}


/* Compute the hash code associaetd with a key*/
int hashFunction(char *key, int size)
{
	int hashCode = 0, i = 0, len = strlen(key);


	/* Hashcode has to be a position in the list of pairs */
	for (i = 0; i < len; i++)
		hashCode = (hashCode + key[i]) % size;

	return hashCode;
}


/* Returns true if this map contains a mapping for the specified key*/
int containsKey(HashMap *map, char *key)
{
	int hashCode = hashFunction(key, map->size);
	Pair *current = map->list[hashCode];

	while (current != NULL) {
		if (!strcmp(key, current->key))
			return 1;
		current = current->next;
	}

	return 0;
}


/* Replaces the entry for the specified key
 * only if it is currently mapped to some value.
 */
int replaceValue(HashMap *map, char *key, char *value)
{
	int hashCode = hashFunction(key, map->size);
	Pair *current = map->list[hashCode];

	while (current && strcmp(key, current->key) > 0)
		current = current->next;

	if (current && !strcmp(key, current->key)) {
		free(current->value);
		current->value = (char *) calloc(sizeof(char),
						strlen(value) + 1);
		memcpy(current->value, value, strlen(value));
	}

	return 1;
}


/* If the specified key is not already associated */
int putIfAbsent(HashMap *map, char *key, char *value)
{
	int hashCode;
	Pair *current = NULL; Pair *last = NULL; Pair *pair = NULL;

	/* Form the pair (key, value) */
	pair = (Pair *) malloc(sizeof(Pair));
	if (!pair) {
		free(pair);
		return ERROR_CODE;
	}

	pair->key = (char *) calloc(sizeof(char), strlen(key) + 1);
	pair->value = (char *) calloc(sizeof(char), strlen(value) + 1);
	if (!pair->key || !pair->value) {
		free(pair->key);
		free(pair->value);
		free(pair);
		return ERROR_CODE;
	}

	memcpy(pair->key, key, strlen(key));
	memcpy(pair->value, value, strlen(value));
	pair->next = NULL;

	/* Get the hashcode and find the position to insert */
	hashCode = hashFunction(key, map->size);
	current = map->list[hashCode];
	last = NULL;

	/* If that hashcode is already used,find the next available position */
	while (current && strcmp(key, current->key) > 0) {
		last = current;
		current = current->next;
	}

	/* Insert the pair in the HashMap, making sure to link the pairs */
	if (current == map->list[hashCode]) {
		pair->next = current;
		map->list[hashCode] = pair;
	} else if (current == NULL)
		last->next = pair;
	else {
		pair->next = current;
		last->next = pair;
	}

	return 1;
}


/* Associates the specified value with the specified key in the map */
int putPair(HashMap *map, char *key, char *value)
{
	int ret;

	if (containsKey(map, key)) {
		/* Update the value for the specified key */
		ret = replaceValue(map, key, value);
	} else {
		/* Insert the pair into the map */
		ret = putIfAbsent(map, key, value);
	}

	return ret;
}


/* Remove an entry from the map */
void removeEntry(HashMap *map, char *key)
{
	int hashCode = hashFunction(key, map->size);

	/* Find the pair in the HashMap that matches the key */
	Pair *current = map->list[hashCode];
	Pair *last = NULL;

	if (!containsKey(map, key))
		DIE(-1, "Error at erasing key");


	while (current) {
		/* Erase the mapping if the pair was found */
		if (!strcmp(key, current->key)) {
			if (last != NULL)
				last->next = current->next;
			else
				map->list[hashCode] = current->next;

			free(current->key);
			free(current->value);
			free(current);
			return;

		}

		last = current;
		current = current->next;
	}
}


/* Returns the value to which the specified key is mapped, */
/* or NULL if this map contains no mapping for the key */
char *getValue(HashMap *map, char *key)
{
	int hashCode = hashFunction(key, map->size);
	Pair *current = map->list[hashCode];

	/* Find the pair in the HashMap that matches the key */
	while (current) {
		/* Replace the value if the pair was found */
		if (!strcmp(key, current->key))
			return current->value;

		current = current->next;
	}

	return NULL;
}


/* Removes all of the mappings and frees space */
void freeMap(HashMap *map)
{
	int i;
	Pair *current = NULL;

	for (i = 0; i < map->size; ++i) {
		if (map->list[i] != NULL) {
			while (map->list[i] != NULL) {
				current = map->list[i];
				map->list[i] = map->list[i]->next;
				current->next = NULL;

				free(current->key);
				free(current->value);
				free(current);
				current = NULL;
			}
		}

		map->list[i] = NULL;
	}

	free(map->list);
	free(map);
}
