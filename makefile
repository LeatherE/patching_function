CFLAGS = -O3 -o ver
SRC = patching.cpp Parser.cpp Parser.h

all:
	g++ -std=c++11 ${CFLAGS} ${SRC}

clean:
	rm -f ver
