CC=clang
CFLAGS=-Wall -Werror -lm -O

all:	mPrimes

clean:
	rm -rf mPrimes primes.map *.o

mPrimes.o: mPrimes.c

mPrimes: mPrimes.o
	$(CC) $(CFLAGS) -o $@ $^

run: mPrimes
	./mPrimes

time: mPrimes
	/usr/bin/time ./mPrimes
