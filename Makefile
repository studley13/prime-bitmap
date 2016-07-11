CC=clang
CFLAGS=-Wall -Werror -O3

all:	mPrimes

clean:
	rm -rf mPrimes primes.map *.o

mPrimes.o: mPrimes.c

mPrimes: mPrimes.o
	$(CC) $(CFLAGS) -lm -o $@ $^

run: mPrimes
	./mPrimes

time: mPrimes
	/usr/bin/time -f "%U - User Time\n%S - System Time\n%P - Processor Utilisation" ./mPrimes

primes.map: mPrimes
	./mPrimes

check: primes.map
