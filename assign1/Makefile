CC = gcc
CFLAGS = -Wall -g -std=c11
INCLUDE = -Iinclude
ASSETS = assets/
SRC = src/
BIN = bin/
OBJ = obj/
TEST = $(BIN)test

all: test

test:  LinkedListAPI.o GEDCOMparser.o GEDCOMutilities.o test.o
	$(CC) $(OBJ)LinkedListAPI.o $(OBJ)GEDCOMparser.o $(OBJ)GEDCOMutilities.o $(OBJ)test.o  -o $(TEST)

LinkedListAPI.o:
	$(CC) $(CLFAGS) $(INCLUDE) -c $(SRC)LinkedListAPI.c -o $(OBJ)LinkedListAPI.o
GEDCOMparser.o:
	$(CC) $(CLFAGS) $(INCLUDE) -c $(SRC)GEDCOMparser.c -o $(OBJ)GEDCOMparser.o
GEDCOMutilities.o:
	$(CC) $(CLFAGS) $(INCLUDE) -c $(SRC)GEDCOMutilities.c -o $(OBJ)GEDCOMutilities.o	
test.o:
	$(CC) $(CLFAGS) $(INCLUDE) -c $(SRC)test.c -o $(OBJ)test.o

run:
	valgrind ./$(TEST)
runLeaks:
	valgrind --leak-check=full  --show-leak-kinds=all ./$(TEST)

clean:
	rm $(BIN)* $(OBJ)*
