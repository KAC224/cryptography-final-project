#include <stdint.h>
#include <stddef.h>
#include "randombytes.h"

// simple xorshift32 PRNG state
static uint32_t rng_state = 0xDEADBEEF;

// generate 32-bit pseudo random number
static uint32_t xorshift32(void) {
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

void randombytes(uint8_t *out, size_t outlen) {
    for (size_t i = 0; i < outlen; i++) {
        uint32_t r = xorshift32();
        out[i] = (uint8_t)(r & 0xFF);
    }
}
