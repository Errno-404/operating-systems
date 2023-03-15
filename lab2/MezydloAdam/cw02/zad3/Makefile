CC=gcc
CFLAGS=-Wall -O2

search: search.c
	$(CC) $(CFLAGS) -o search search.c

clean:
	rm -f *.o search


test:
	./search
	
wc:
	wc --bytes *