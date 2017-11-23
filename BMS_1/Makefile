CC=g++
FLAGS= -std=c++11 -Wall -pedantic -Wextra -lm -lcppunit
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
	main.h \
	UTM.h \
	Makefile

SOURCES_TEST_BTSDATA= BTSLoadData.cpp \
	 BTSData.cpp \
	 Calculation.cpp \
	 Distance.cpp \
	 mainTestBTSData.cpp

SOURCES_TEST_GPS= BTSLoadData.cpp \
	 BTSData.cpp \
	 Calculation.cpp \
	 Distance.cpp \
	 mainTestGPS.cpp

SOURCES_TEST_HATA= BTSLoadData.cpp \
	 BTSData.cpp \
	 Calculation.cpp \
	 Distance.cpp \
	 mainTestHata.cpp

SOURCES_TEST_APP= BTSLoadData.cpp \
	 BTSData.cpp \
	 Calculation.cpp \
	 Distance.cpp \
	 mainTestApp.cpp

all:
	$(CC) $(FLAGS) $(SOURCES) -o p1 -L/usr/local/lib -L/usr/local/lib64 -lPocoFoundation -lPocoUtil

clean:
	/bin/rm -rf p1 $(LOGIN).zip

zip:
	zip $(LOGIN).zip $(SOURCES) $(HEADERS)

test1:
	$(CC) $(FLAGS) $(SOURCES_TEST_BTSDATA) -o proj1 -L/usr/local/lib -L/usr/local/lib64 -lPocoFoundation -lPocoUtil

test2:
	$(CC) $(FLAGS) $(SOURCES_TEST_GPS) -o proj1 -L/usr/local/lib -L/usr/local/lib64 -lPocoFoundation -lPocoUtil

test3:
	$(CC) $(FLAGS) $(SOURCES_TEST_HATA) -o proj1 -L/usr/local/lib -L/usr/local/lib64 -lPocoFoundation -lPocoUtil

test4:
	$(CC) $(FLAGS) $(SOURCES_TEST_APP) -o proj1 -L/usr/local/lib -L/usr/local/lib64 -lPocoFoundation -lPocoUtil
