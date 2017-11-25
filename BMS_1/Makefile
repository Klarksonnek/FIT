CC=g++
FLAGS= -std=c++11 -Wall -pedantic -Wextra -lm
LOGIN=xnecas24
SOURCES= BTSLoadData.cpp \
	BTSData.cpp \
	Coordinates.cpp \
	CustomException.cpp \
	Distance.cpp \
	main.cpp

HEADERS= BTSLoadData.h \
	BTSData.h \
	Coordinates.h \
	CustomException.h \
	Distance.h \
	UTM.h \
	Makefile

all:
	$(CC) $(FLAGS) $(SOURCES) -o p1 -L/usr/local/lib64 -lPocoFoundation -lPocoUtil

clean:
	/bin/rm -rf p1 $(LOGIN).zip

zip:
	zip $(LOGIN).zip $(SOURCES) $(HEADERS)
