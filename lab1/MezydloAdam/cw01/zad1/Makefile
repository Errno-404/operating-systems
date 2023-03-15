CC=gcc
CFLAGS = -Wall

all: static shared

static: 
	$(CC) $(CFLAGS) -c wclib.c
	ar rcs libwclib.a wclib.o

shared:	
	$(CC) $(CFLAGS) -fPIC -c wclib.c
	$(CC) -shared wclib.o -o libwclib.so
	
clean:
	rm -f *.o *.a *.so



CFLAGS = -Wall
CC = gcc
