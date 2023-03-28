CC=gcc
CFLAGS=-Wall -O2

all: main_exec.c main_fork.c
	$(CC) $(CFLAGS) -o main_exec main_exec.c
	$(CC) $(CFLAGS) -o main_fork main_fork.c

clean:
	rm -rf main_exec main_fork

ignore:
	echo "exec:" && ./main_exec ignore 
	echo "fork:" && ./main_fork ignore
	

handler:
	echo "fork" && ./main_fork handler

mask:
	echo "exec:" && ./main_exec mask
	echo "fork:" && ./main_fork mask

pending:
	echo "exec:" && ./main_exec pending
	echo "fork:" && ./main_fork pending