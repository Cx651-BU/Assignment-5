all: cli test

CC=gcc
CFLAGS=-I. -lm -g

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

cli: cli.o kernel.o loader.o
	$(CC) -o $@ $^ $(CFLAGS)

test: test.o kernel.o loader.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f test
	rm -f cli