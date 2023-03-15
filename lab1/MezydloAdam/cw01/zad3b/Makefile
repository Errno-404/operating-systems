CFLAGS = -Wall
CC = gcc
O=-O0

all: static shared dll

static: 
	$(CC) $(CFLAGS) $(O) -c  main.c
	$(CC) $(CFLAGS) $(O) -c  wclib.c
	
	ar rcs libwclib.a wclib.o
	
	$(CC) -static main.o -o main_stat -L. -lwclib


shared:
	$(CC) $(CFLAGS) $(O) -c  main.c
	$(CC) $(CFLAGS) $(O) -fPIC -c wclib.c
	
	$(CC) -shared wclib.o -o libwclib.so
	$(CC) main.o -o main_shared -L. -Wl,-rpath=. -lwclib

dll:
	$(CC) $(CFLAGS) $(O) -c main_dll.c
	$(CC) $(CFLAGS) $(O) -fPIC -c wclib.c
	
	$(CC) -shared wclib.o -o libwclib.so
	$(CC) main_dll.o -o main_dll -L. -Wl,-rpath=. -ldl -lwclib



clean:
	rm -f *.o  *.so *.a main_stat main_shared main_dll
	

test_static: static
	./main_stat < input.txt > results_static_$(O).txt

test_shared: shared
	./main_shared < input.txt > results_shared_$(O).txt

test_dll: dll
	./main_dll < input.txt > results_dynamic_$(O).txt