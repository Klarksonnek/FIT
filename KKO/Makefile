CC=g++                     # překladač jazyka C++
FLAGS= -Wall -pedantic -g  # parametry překladace
COMPILE = $(CC) $(FLAGS)

all: main

main: 
	$(CC) $(FLAGS) gif2bmp.c -std=c++11  main.c -o gif2bmp

debug:
	$(CC) gif2bmp.c main.c -o gif2bmp

clean:
	rm -f gif2bmp
	
