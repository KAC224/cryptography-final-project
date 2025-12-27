#include <stdint.h>
#include "rng.h"

// Very simple LCG. NOT cryptographically secure.
// Fine for benchmarking on ESP32.
static uint32_t rng_state = 0x12345678u;

int randombytes(unsigned char *x, unsigned long long xlen) {
    while (xlen > 0) {
        rng_state = 1664525u * rng_state + 1013904223u;
        *x = (unsigned char)(rng_state >> 24);
        x++;
        xlen--;
    }
    return 0;
}
