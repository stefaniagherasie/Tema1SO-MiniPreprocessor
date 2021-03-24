CC = gcc
CFLAGS = -Wextra -Wall -g

build: so-cpp.o hashmap.o directory.o
	$(CC) -o so-cpp so-cpp.o hashmap.o directory.o

so-cpp.o: so-cpp.c
	$(CC) $(CFLAGS) -c so-cpp.c

hashmap.o: hashmap.c
	$(CC) $(CFLAGS) -c hashmap.c

directory.o: directory.c
	$(CC) $(CFLAGS) -c directory.c

clean:
	rm -f *.o so-cpp
