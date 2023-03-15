CC=gcc
CFLAGS=-Wall -O2

search: search.c
	$(CC) $(CFLAGS) -o search search.c

clean:
	rm -f *.o search



test:
	cd .. && ./zad4/search zad4

find:
	cd .. && find zad4 | xargs wc --bytes
	
