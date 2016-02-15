CC = gcc
CCFLAGS = -Wall -O3 -g
LDLIBS = -lm

all: packCircles

packCircles: makefile packCircles.c
	$(CC) $(CCFLAGS) -o packCircles packCircles.c $(LDLIBS) 

clean: 
	rm -f packCircles

.PHONY: clean 
