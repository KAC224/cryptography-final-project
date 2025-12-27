#ifndef DILITHIUM_WRAPPER_H
#define DILITHIUM_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

/*
 * Include Dilithium headers (headers only). 
 * sign.h (and the other headers it includes) define the actual crypto APIs.
 * Do NOT include .c files here.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "sign.h"   /* provides crypto_sign_keypair / crypto_sign_signature / crypto_sign_verify */
#include "params.h" /* may define parameter sizes */

/* If the CRYPTO_* macros are missing in your headers, provide safe fallbacks.
   Adjust sizes to match your chosen Dilithium variant if necessary. */
#ifndef CRYPTO_PUBLICKEYBYTES
#define CRYPTO_PUBLICKEYBYTES 1472
#endif
#ifndef CRYPTO_SECRETKEYBYTES
#define CRYPTO_SECRETKEYBYTES 3504
#endif
#ifndef CRYPTO_BYTES
#define CRYPTO_BYTES 2701
#endif

/* Arduino-facing wrapper API (match calls in your .ino) */
int dilithium_keypair(uint8_t *pk, uint8_t *sk);

/* Note: .ino calls this as:
     dilithium_sign(sig, &siglen, msg, msglen, sk);
   wrapper will call the extended API with ctx=NULL, ctxlen=0 */
int dilithium_sign(uint8_t *sig, size_t *siglen,
                   const uint8_t *m, size_t mlen,
                   const uint8_t *sk);

/* Called from .ino as:
     dilithium_verify(sig, siglen, msg, msglen, pk);
   wrapper will call extended verify with ctx=NULL, ctxlen=0 */
int dilithium_verify(const uint8_t *sig, size_t siglen,
                     const uint8_t *m, size_t mlen,
                     const uint8_t *pk);

#ifdef __cplusplus
}
#endif

#endif /* DILITHIUM_WRAPPER_H */

