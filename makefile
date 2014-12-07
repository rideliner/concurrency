CC = g++
CFLAGS = -pthread -std=c++11 -Iinclude -Wall
LINK = 

OBJS = build/pool.o build/worker.o
LIBS = lib/libconcurrency.a
DIRS = lib build

all: | $(OBJS) $(LIBS)

clean:
	-rm -rf $(DIRS)

lib:
	-mkdir $@

build:
	-mkdir $@

build/%.o: src/%.cpp | build
	$(CC) -c -o $@ $^ $(CFLAGS)

lib/libconcurrency.a: $(OBJS) | lib
	ar crf $@ $^

.PHONY: all clean
