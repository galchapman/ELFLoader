CC=gcc
CFLAGS=-std=c17 -Wall

all: build/main
	./$^

build/test.o: test.s
	nasm -f elf64 -o $@ $^

build/test.a: build/test.o
	ar rcs $@ $^

build/main: build/test.o
	ld $^ -o $@
