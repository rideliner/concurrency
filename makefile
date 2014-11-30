CC = g++
CFLAGS = -pthread -std=c++11 -Iinclude -Wall
LINK = 

OBJS = build/pool.o build/worker.o
LIBS = libconcurrency.a

all: | $(OBJS) $(LIBS)

clean:
	-rm -f build/*

build/%.o: src/%.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)

libconcurrency.a: $(OBJS)
	ar crf lib/$@ $^

.PHONY: all clean
