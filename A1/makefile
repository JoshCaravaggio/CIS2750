CC = gcc
CFLAGS = -Wall -g -std=c11 
LIB = ar cr
INCLUDE = -Iinclude
ASSETS = assets/
SRC = src/
BIN = bin/

all: list parser

list: 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o
	$(LIB) $(BIN)liblist.a $(BIN)LinkedListAPI.o

parser:
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC)GEDCOMparser.c -o $(BIN)GEDCOMparser.o
	$(LIB) $(BIN)libparser.a $(BIN)GEDCOMparser.o

clean:
	rm $(BIN)*.o $(BIN)*.a
