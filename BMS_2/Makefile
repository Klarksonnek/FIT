CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2
LIBS=-lm
LOGIN=xnecas24

all: bms2A bms2B


bms2A: bms2A.cpp Modulator.cpp CustomException.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

bms2B: bms2B.cpp Demodulator.cpp CustomException.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms2A bms2B

FILES=sndfile.hh \
    sndfile.h \
    bms2A.cpp \
    bms2B.cpp \
    CustomException.h \
    CustomException.cpp \
    Modulator.h \
    Modulator.cpp \
    Demodulator.h \
    Demodulator.cpp \
    Makefile

zip:
	zip $(LOGIN).zip $(FILES)
