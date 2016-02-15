CC = gcc
CCFLAGS = -O3 -g
LDLIBS = -lm

all: packCircles

packCircles: makefile 
	$(CC) $(CCFLAGS) -o packCircles packCircles.c $(LDLIBS) 

clean: 
	rm -f packCircles

.PHONY: clean 
