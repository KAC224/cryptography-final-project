#include <Arduino.h>

extern "C" {
  #include "api.h"      // SABER api: CRYPTO_* and crypto_kem_* functions
}

#define SABER_TASK_STACK   60000   // bytes
#define SABER_TASK_CORE    1       // run on core 1
#define RUNS               100     // number of timing samples

// ------------ helper: compute stats ------------
struct Stats {
  double avg_valid;
  double avg_invalid;
  uint32_t min_valid;
  uint32_t max_valid;
  uint32_t min_invalid;
  uint32_t max_invalid;
};

void printStats(const Stats &s) {
  Serial.println("===== SABER timing side-channel test =====");
  Serial.print("Runs = ");
  Serial.println(RUNS);

  Serial.println("\n[Valid ciphertext decapsulation]");
  Serial.print("  avg (us): "); Serial.println(s.avg_valid, 3);
  Serial.print("  min (us): "); Serial.println(s.min_valid);
  Serial.print("  max (us): "); Serial.println(s.max_valid);

  Serial.println("\n[Invalid ciphertext decapsulation]");
  Serial.print("  avg (us): "); Serial.println(s.avg_invalid, 3);
  Serial.print("  min (us): "); Serial.println(s.min_invalid);
  Serial.print("  max (us): "); Serial.println(s.max_invalid);

  Serial.println("\n[Difference]");
  Serial.print("  avg_invalid - avg_valid (us): ");
  Serial.println(s.avg_invalid - s.avg_valid, 3);
}

// ------------ main SABER side-channel task ------------
void saberSideChannelTask(void *pv) {
  Serial.println("SABER side-channel timing test starting...");

  // Key material
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];

  // Buffers
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t ct_bad[CRYPTO_CIPHERTEXTBYTES];
  uint8_t ss[CRYPTO_BYTES];
  uint8_t ss_dummy[CRYPTO_BYTES];

  // Generate keypair once
  int rc = crypto_kem_keypair(pk, sk);
  if (rc != 0) {
    Serial.print("Keypair generation failed, rc = ");
    Serial.println(rc);
    vTaskDelete(NULL);
    return;
  }
  Serial.println("Keypair generated.");

  double sum_valid = 0.0;
  double sum_invalid = 0.0;
  uint32_t min_valid = UINT32_MAX, max_valid = 0;
  uint32_t min_invalid = UINT32_MAX, max_invalid = 0;

  // Main measurement loop
  for (int i = 0; i < RUNS; i++) {
    // --- 1. Generate a fresh valid ciphertext ---
    rc = crypto_kem_enc(ct, ss, pk);
    if (rc != 0) {
      Serial.print("Encapsulation failed, rc = ");
      Serial.println(rc);
      vTaskDelete(NULL);
      return;
    }

    // --- 2. Measure decapsulation time for VALID ciphertext ---
    uint32_t t1 = micros();
    crypto_kem_dec(ss_dummy, ct, sk);
    uint32_t t2 = micros();
    uint32_t dt_valid = t2 - t1;

    // --- 3. Make an INVALID ciphertext by flipping one byte ---
    memcpy(ct_bad, ct, CRYPTO_CIPHERTEXTBYTES);
    // flip a different byte each iteration just to avoid any weird caching
    size_t idx = i % CRYPTO_CIPHERTEXTBYTES;
    ct_bad[idx] ^= 0x01;

    // --- 4. Measure decapsulation time for INVALID ciphertext ---
    t1 = micros();
    crypto_kem_dec(ss_dummy, ct_bad, sk);
    t2 = micros();
    uint32_t dt_invalid = t2 - t1;

    // --- 5. Update stats ---
    sum_valid   += dt_valid;
    sum_invalid += dt_invalid;

    if (dt_valid < min_valid)   min_valid   = dt_valid;
    if (dt_valid > max_valid)   max_valid   = dt_valid;
    if (dt_invalid < min_invalid) min_invalid = dt_invalid;
    if (dt_invalid > max_invalid) max_invalid = dt_invalid;
  }

  Stats s;
  s.avg_valid   = sum_valid   / RUNS;
  s.avg_invalid = sum_invalid / RUNS;
  s.min_valid   = min_valid;
  s.max_valid   = max_valid;
  s.min_invalid = min_invalid;
  s.max_invalid = max_invalid;

  printStats(s);

  Serial.println("\nSABER side-channel test finished.");
  vTaskDelete(NULL);   // kill task when done
}

// ------------ Arduino setup / loop ------------
void setup() {
  Serial.begin(115200);
  delay(2000); // let serial settle

  Serial.println();
  Serial.println("=== ESP32 SABER Side-Channel Timing Experiment ===");

  xTaskCreatePinnedToCore(
    saberSideChannelTask,
    "SABER_SC",
    SABER_TASK_STACK,
    NULL,
    1,
    NULL,
    SABER_TASK_CORE
  );
}

void loop() {
  // nothing here; work is done in FreeRTOS task
}
