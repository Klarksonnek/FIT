# Projekt: IOS - 2. projekt (Building H20 Problem)
# Autor:   Klara Necasova (xnecas24@stud.fit.vutbr.cz)
# Datum:   Kveten 2015

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS=-pthread
PROJ=h2o

$(ALL): h2o

$(PROJ): $(PROJ).c
	$(CC) $(CFLAGS) $(PROJ).c -o $(PROJ) -pthread

.PHONY:
run:
	rm -f $(PROJ).out
	./$(PROJ) 1 0 0 0
	
pack:
	zip xnecas24.zip h2o.c Makefile 

clean:
	rm -f h2o 
	rm -f h2o.out
	
