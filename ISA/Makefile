# Projekt: ISA: Programovani sitove sluzby - Analyzator sitoveho provozu
# Soubor:  Makefile
# Autor:   Klara Necasova
# Email:   xnecas24@stud.fit.vutbr.cz
# Datum:   listopad 2016

# prekladac jazyka C++ 
CC=g++

# parametry prekladace                   
FLAGS=-std=c++98 -Wall -Wextra -pedantic -g 

# login
LOGIN=xnecas24
SRC_FILES=analyzer.cpp Converter.cpp PcapReader.cpp ProcessParams.cpp
HEADER_FILES=global.h Converter.h PcapReader.h ProcessParams.h

# pravidla pro preklad
all: analyzer.o PcapReader.o ProcessParams.o Converter.o
	$(CC) $(FLAGS) analyzer.o PcapReader.o ProcessParams.o Converter.o -o analyzer

make clean:
	rm analyzer analyzer.o PcapReader.o ProcessParams.o Converter.o
    
tar:
	tar rvf $(LOGIN).tar $(SRC_FILES) $(HEADER_FILES) README Makefile manual.pdf
