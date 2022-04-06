#Makefile for stairs.c

CC = gcc
OUT = stairs
SRC = stairs.c
all: 	stairs

stairs: stairs.c 
	$(CC) -Wall -pthread -std=c99 -o $(OUT) $(SRC)
	chmod 700 $(OUT)
	 ./$(OUT)

	
clean:
	rm -f $(OUT)
