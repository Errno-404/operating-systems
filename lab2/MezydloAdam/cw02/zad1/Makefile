CC=gcc
CFLAGS=-Wall -O2

all: sys lib

lib:
	$(CC) $(CFLAGS) -o lib_imp lib_imp.c

sys:
	$(CC) $(CFLAGS) -o sys_imp sys_imp.c


clean:
	rm -f sys_imp lib_imp *.o output_lib.txt output_sys.txt output_tr.txt


test:
	./sys_imp i a input output_sys.txt 
	./lib_imp i a input output_lib.txt 

	diff output_lib.txt output_sys.txt 


	tr 'i' 'a' <input> output_tr.txt 

	diff output_tr.txt output_sys.txt 
	diff output_tr.txt output_lib.txt 



