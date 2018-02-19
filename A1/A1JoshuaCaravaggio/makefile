CC = gcc
CFLAGS = -Wall -g -std=c11 
LIB = ar cr
INCLUDE = -Iinclude
ASSETS = assets/
SRC = src/
BIN = bin/
TEST = $(BIN)test

all: list parser

test:   list parser test.o 
	$(CC) $(CFLAGS) $(BIN)LinkedListAPI.o $(BIN)GEDCOMparser.o  $(BIN)test.o  -o $(TEST)
list: 
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o
	$(LIB) $(BIN)liblist.a $(BIN)LinkedListAPI.o
test.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC)test.c -o $(BIN)test.o	

parser:
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC)GEDCOMparser.c -o $(BIN)GEDCOMparser.o
	$(LIB) $(BIN)libparser.a $(BIN)GEDCOMparser.o
run:
	 ./$(TEST)
	
runLeaks:
	valgrind --leak-check=full  --show-leak-kinds=all --track-origins=yes ./$(TEST)
	

clean:
	rm $(BIN)*.o $(BIN)*.a
