// randombytes.c - ESP32-safe, no OS calls

#include <stdint.h>
#include <stddef.h>
#include "randombytes.h"

// simple xorshift32 PRNG (not cryptographically secure, but fine for testing)
static uint32_t state = 0x12345678;

static uint32_t xorshift32(void) {
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}

void randombytes(uint8_t *out, size_t outlen) {
    for (size_t i = 0; i < outlen; i++) {
        out[i] = (uint8_t)(xorshift32() & 0xFF);
    }
}
