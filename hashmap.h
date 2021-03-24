#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"


typedef struct Pair {
	char *key;
	char *value;
	struct Pair *next;
} Pair;


typedef struct HashMap {
	int size;
	struct Pair **list;
} HashMap;


/* Allocate space for the HashMap structure */
HashMap *createMap(int size);

/* Compute the hash code associaetd with a key*/
int hashFunction(char *key, int size);

/* Returns true if this map contains a mapping for the specified key*/
int containsKey(HashMap *map, char *key);

/* Replaces the entry for the specified key
 * only if it is currently mapped to some value.
 */
int replaceValue(HashMap *map, char *key, char *value);

/* If the specified key is not already associated */
/* with a value it is associated with the given value */
int putIfAbsent(HashMap *map, char *key, char *value);

/* Associates the specified value with the specified key in the map */
int putPair(HashMap *map, char *key, char *value);

/* Remove an entry by the key */
void removeEntry(HashMap *map, char *key);

/* Returns the value to which the specified key is mapped, */
/* or NULL if this map contains no mapping for the key */
char *getValue(HashMap *map, char *key);

/* Removes all of the mappings and frees space */
void freeMap(HashMap *map);
