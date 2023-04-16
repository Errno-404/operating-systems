CC=gcc
CFLAGS=-Wall -O2


all:
	$(CC) $(CFLAGS) main.c -o main
	$(CC) $(CFLAGS) integrate.c -o integrate


clean:
	rm main integrate

test:
	./main 0.00001 20