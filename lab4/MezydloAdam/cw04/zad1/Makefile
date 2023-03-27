CC=gcc
CFLAGS=-Wall -O2

all: main.c other.c
	$(CC) $(CFLAGS) -o main main.c
	$(CC) $(CFLAGS) -o other other.c

clean:
	rm -rf main other

ignore:
	./main ignore

handler:
	./main handler

mask:
	./main mask

pending:
	./main pending