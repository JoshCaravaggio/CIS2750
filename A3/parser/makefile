CC=gcc
CFLAGS= -std=c11

#UNAME Shell Variable
UNAME_S := $(shell uname -s)

# OS X Dependant Variables
ifeq ($(UNAME_S), Darwin)
    LIB := sharedLib.dylib
# GNU/Linux Depedant Variables
else ifeq ($(UNAME_S), Linux)
    LIB := sharedLib.so
endif

$(LIB): sharedLib.o LinkedListAPI.o
	$(CC) -shared -o ../$(LIB) GEDCOMparser.o LinkedListAPI.o

sharedLib.o: src/GEDCOMparser.c
	$(CC) $(CFLAGS) -Iinclude -c -fpic src/GEDCOMparser.c

LinkedListAPI.o: src/LinkedListAPI.c
	gcc -Iinclude -c src/LinkedListAPI.c -o LinkedListAPI.o -Wall -std=c11 -g

clean:
	rm $(LIB) *.o