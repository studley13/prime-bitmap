CC=clang
CFLAGS=-Wall -Werror -O

all:	mPrimes

mPrimes.o: mPrimes.c

mPrimes: mPrimes.o
	$(CC) $(CFLAGS) -o $@ $^

run: mPrimes
	./mPrimes

time: mPrimes
	/usr/bin/time ./mPrimes
