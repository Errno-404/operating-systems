CC=gcc
CFLAGS=-Wall -O2

all: byte block
	

byte:
	$(CC) $(CFLAGS) -o reverse_byte reverse_byte.c

block:
	$(CC) $(CFLAGS) -o reverse_block reverse_block.c


clean:
	rm -f reverse_block reverse_byte *.o output_block output_block_rev output_byte output_byte_rev


test:
	./reverse_block input output_block
	./reverse_block output_block output_block_rev

	diff input output_block_rev


	./reverse_byte input output_byte
	./reverse_byte output_byte output_byte_rev


	diff input output_byte_rev
