# File: Makefile
# Author: Bc. Klara Necasova
# Login: xnecas24
# Date: 16. 2. 2018
# Description: Project compilation

CC=g++                     # C++ compiler
FLAGS= -Wall -pedantic -g  # compiler parameters
COMPILE = $(CC) $(FLAGS)

all: main

main: 
	$(CC) $(FLAGS) gif2bmp.c -std=c++11  main.c -o gif2bmp

debug:
	$(CC) gif2bmp.c main.c -o gif2bmp

clean:
	rm -f gif2bmp
