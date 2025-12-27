#include <Arduino.h>

extern "C" {
  #include "api.h"
  #include "randombytes.h"
  #include "kem.h"
  #include "params.h"
  #include "poly.h"
  #include "polyvec.h"
  #include "reduce.h"
  #include "ntt.h"
  #include "indcpa.h"
  #include "fips202.h"
  #include "symmetric.h"
}

#define KYBER_TASK_STACK  45000   // 45 KB stack
#define KYBER_TASK_CORE   1       // Run on Core 1

// -------- HEX PRINT HELPER -------- //
void printHex(const char *label, const uint8_t *buf, size_t len) {
  Serial.print(label);
  Serial.print(" (");
  Serial.print(len);
  Serial.println(" bytes):");

  for (size_t i = 0; i < len; i++) {
    if (i % 16 == 0) Serial.print("  "); // indent rows
    if (buf[i] < 16) Serial.print("0");
    Serial.print(buf[i], HEX);
    Serial.print(" ");

    if ((i + 1) % 16 == 0) Serial.println();
  }
  Serial.println("\n");
}

// -------- KYBER TASK -------- //
void kyberTask(void *pv) {
  Serial.println("Kyber task running...");

  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t ss1[CRYPTO_BYTES];
  uint8_t ss2[CRYPTO_BYTES];

  Serial.println("Generating keypair...");
  crypto_kem_keypair(pk, sk);

  printHex("Public Key", pk, CRYPTO_PUBLICKEYBYTES);
  printHex("Secret Key", sk, CRYPTO_SECRETKEYBYTES);

  Serial.println("Encapsulating...");
  crypto_kem_enc(ct, ss1, pk);

  printHex("Ciphertext", ct, CRYPTO_CIPHERTEXTBYTES);
  printHex("Shared Secret (ss1)", ss1, CRYPTO_BYTES);

  Serial.println("Decapsulating...");
  crypto_kem_dec(ss2, ct, sk);

  printHex("Shared Secret (ss2)", ss2, CRYPTO_BYTES);

  bool match = true;
  for (int i = 0; i < CRYPTO_BYTES; i++) {
    if (ss1[i] != ss2[i]) match = false;
  }

  Serial.print("Shared secret match: ");
  Serial.println(match ? "YES" : "NO");

  vTaskDelete(NULL);
}

// -------- SETUP -------- //
void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("Setup started");

  xTaskCreatePinnedToCore(
    kyberTask,
    "KyberTask",
    KYBER_TASK_STACK,
    NULL,
    1,
    NULL,
    KYBER_TASK_CORE
  );

  Serial.println("Kyber task created.");
}

void loop() {}


