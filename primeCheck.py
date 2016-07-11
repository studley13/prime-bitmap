#!/usr/bin/env python

def checkPrime(bitmap, number):
    block = number / 8
    shift = number % 8

    return (((ord(bitmap[block]) >> shift) & 0x1) != 0)

data = file("primes.map", 'r').read()

num = raw_input("Enter a number: ")

while ((len(num) > 0) and (num.isdigit())):

    n = int(num, 0)
    if (checkPrime(data, n)):
        print("{} is a prime number".format(n))
    else:
        print("{} is not a prime number".format(n))

    num = raw_input("\nEnter a number: ")
