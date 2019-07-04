GCC=g++
CFLAGS=-std=c++11
LIB=-lpcap

all:
	$(GCC) pds-dhcpstarve.cpp Common.cpp ClientMessage.cpp Util.cpp \
	-o pds-dhcpstarve $(CFLAGS) $(LIB)

	$(GCC) pds-dhcprogue.cpp Common.cpp ServerMessage.cpp Util.cpp \
	-o pds-dhcprogue $(CFLAGS) $(LIB)

clean:
	/bin/rm -rf pds-dhcpstarve pds-dhcprouge

