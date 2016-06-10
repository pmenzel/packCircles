CC = gcc
CCFLAGS = -Wall -Wconversion -O3 -g -ansi -pedantic -std=gnu99
LDLIBS = -lm

all: packCircles

packCircles: makefile packCircles.c packCircles.h
	$(CC) $(CCFLAGS) -o packCircles packCircles.c $(LDLIBS) 

clean: 
	rm -f packCircles

.PHONY: clean 
