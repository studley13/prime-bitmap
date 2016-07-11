#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE (!FALSE)

#define PRIME_FILE    "primes.map"
#define PRIME_SIZE    0x100000000

// Comment out to prevent printing
#define VERBOSE

typedef unsigned long long int block;

typedef struct _mem {
    int fd;
    block *data;
    long long int size;
} mem;

mem openBitmap(char *file, size_t length);
void closeBitmap(mem memory);
void genPrimes(mem bitmap);
void writeBitmap(mem bitmap);
void readBitmap(mem bitmap);
void flushBlock(mem bitmap, long long int index, unsigned char data);
void setBit(mem bitmap, long long int index, unsigned char data);
int getBit(mem bitmap, long long int index);

int main (int argc, char *argv[]) {
    mem bitmap = openBitmap(PRIME_FILE, PRIME_SIZE);

    genPrimes(bitmap);

    writeBitmap(bitmap);

    closeBitmap(bitmap);
    return EXIT_SUCCESS;
}

mem openBitmap(char *file, size_t length) {
    mem newMemory = {-1, NULL, -1};
    size_t byteLength = (length / 8) + !!(length % 8);

    // Open the file
    newMemory.fd = open(file, O_RDWR|O_CREAT, 0644);

    if (newMemory.fd > 0) {
        // Allocate file size
        posix_fallocate(newMemory.fd, 0, byteLength);

        newMemory.size = length;
        newMemory.data = malloc(byteLength);

        if (newMemory.data == NULL) {
            close(newMemory.fd);
            newMemory.fd = -1;
            newMemory.size = -1;
        }
    }

    return newMemory;
}

void closeBitmap(mem memory) {
    if (memory.fd >= 0) {
        free(memory.data);
        memory.data = NULL;
        memory.size = -1;
        close(memory.fd);
        memory.fd = -1;
    }
}

void genPrimes(mem bitmap) {
    long long int check = 0;
    long long int multiple = 0;
    long long int dispCond = 1;

    if (bitmap.fd >= 0) {

        // initialise data
        for (check = 0; check < PRIME_SIZE; check++) {
            if ((check % (sizeof(block) * 8)) == 0) {
                flushBlock(bitmap, check, 1);
            }

            #ifdef VERBOSE
            if ((check % (16 * 1024 * 1024)) == 0) {
                fprintf(stderr, "\033[G\033[KCleaned %10llx bits", check);
                fflush(stderr);
            }
            #endif
        }
        
        #ifdef VERBOSE
        printf("\033[G\033[KCleaned %10llx bits\n", check);
        #endif

        // run seive
        for (check = 0; check < PRIME_SIZE; check++) {
            if ((check == 0) || (check == 1)) {
                setBit(bitmap, check, 0);
            } else if (getBit(bitmap, check)) {

                #ifdef VERBOSE
                if (check > dispCond) {
                    fprintf(stderr, "\033[G\033[KPrime found: %12lld", check);
                    fflush(stderr);
                    dispCond <<= 1;
                }
                #endif

                for (multiple = check + check; multiple < PRIME_SIZE; multiple += check) {
                    setBit(bitmap, multiple, 0);
                }
            }
        }

        #ifdef VERBOSE
        printf("\033[G\033[KPrimes found.\n");
        #endif

    }

    return;
}

void writeBitmap(mem bitmap) {
    long long int byteCount = (bitmap.size / 8) + !!(bitmap.size % 8);

    if (bitmap >= 0) {
        lseek(bitmap.fd, 0, SEEK_SET);
        write(bitmap.fd, bitmap.data, byteCount);
    }

    return;
}

void readBitmap(mem bitmap) {
    long long int byteCount = (bitmap.size / 8) + !!(bitmap.size % 8);

    if (bitmap.fd >= 0) {
        lseek(bitmap.fd, 0, SEEK_SET);
        read(bitmap.fd, bitmap.data, byteCount);
    }

    return;
}

void flushBlock(mem bitmap, long long int index, unsigned char data) {
    block blockData = 0x0;
    long long int blockIndex = index / (sizeof(block) * 8);

    if (bitmap.fd >= 0) {
        if (data) {
            blockData = ~0x0;
        }

        bitmap.data[blockIndex] = blockData;
    }

    return;
}

void setBit(mem bitmap, long long int index, unsigned char data) {
    long long int blockIndex = index / (sizeof(block) * 8);
    short bitOffset = index % (sizeof(block) * 8);
    block byteData;
    block modifier;

    if (bitmap.fd >= 0) {
        byteData = bitmap.data[blockIndex];
        modifier = (block)0x1 << bitOffset;

        if (data) {
            byteData |= modifier;
        } else {
            byteData &= (~modifier);
        }

        bitmap.data[blockIndex] = byteData;
    }

    return;
}

int getBit(mem bitmap, long long int index) {
    long long int byte = index / (sizeof(block) * 8);
    short bitOffset = index % (sizeof(block) * 8);
    block byteData;
    block bitData = 0;

    if (bitmap.fd >= 0) {
        block byteData = bitmap.data[byte];
        block bitData = (byteData >> bitOffset) & 0x1;
    }

    return !!bitData;
}
