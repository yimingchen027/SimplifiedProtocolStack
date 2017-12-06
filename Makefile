CC=g++
FLAGS=-g -std=gnu++11 -Wall -Wextra
LIBS=-lpthread

all: ppm ppp threadpool.o

ppm: ppm.cc threadpool.o
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

ppp: ppp.cpp
	$(CC) $(FLAGS) -o $@ $^ $(LIBS)

threadpool.o: threadpool.cpp
	$(CC) $(FLAGS) -o $@ -c $^

clean:
	rm -f threadpool.o ppm ppp
