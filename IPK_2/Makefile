# Projekt: IPK - projekt 2: Prenos souboru
# Autor:   Klara Necasova
# Datum:   duben 2016
 
CC=g++                     # prekladac jazyka C++
FLAGS= -Wall -pedantic -g  # parametry prekladace

all: common.cpp server.cpp client.cpp
	$(CC) $(FLAGS) common.cpp client.cpp -o client
	$(CC) $(FLAGS) common.cpp server.cpp -o server


make clean:
	rm client server