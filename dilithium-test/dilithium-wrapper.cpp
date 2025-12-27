#include "dilithium-wrapper.h"

/* The real implementations are in the Dilithium C code.
   We call the header-declared functions:
     - crypto_sign_keypair(pk, sk)
     - crypto_sign_signature(sig, siglen, msg, msglen, ctx, ctxlen, sk)
     - crypto_sign_verify(sig, siglen, msg, msglen, ctx, ctxlen, pk)

   We set ctx = NULL and ctxlen = 0 so your simple .ino API works unchanged.
*/

extern "C" {

/* Keypair */
int dilithium_keypair(uint8_t *pk, uint8_t *sk) {
    return crypto_sign_keypair(pk, sk);
}

/* Sign (matching .ino usage) */
int dilithium_sign(uint8_t *sig, size_t *siglen,
                   const uint8_t *m, size_t mlen,
                   const uint8_t *sk) {
    const uint8_t *ctx = NULL;
    size_t ctxlen = 0;
    return crypto_sign_signature(sig, siglen, m, mlen, ctx, ctxlen, sk);
}

/* Verify (matching .ino usage) */
int dilithium_verify(const uint8_t *sig, size_t siglen,
                     const uint8_t *m, size_t mlen,
                     const uint8_t *pk) {
    const uint8_t *ctx = NULL;
    size_t ctxlen = 0;
    return crypto_sign_verify(sig, siglen, m, mlen, ctx, ctxlen, pk);
}

} /* extern "C" */
