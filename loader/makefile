CC=gcc
CFLAGS=-std=c17 -g -Wall -D_POSIX_C_SOURCE=200809L

all: build/main
	./build/main

build/loader.o: loader.c loader.h
	$(CC) $(CFLAGS) -c loader.c -o $@

build/main: main.c build/loader.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm build/main
	rm build/loader.o