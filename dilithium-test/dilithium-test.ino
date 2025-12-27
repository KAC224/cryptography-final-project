#include <Arduino.h>
#include "sign.h"               
#include "dilithium-wrapper.h"

#define DILITHIUM_TASK_STACK (64 * 1024)
#define DILITHIUM_TASK_PRIO  5

TaskHandle_t dilithiumTaskHandle;

void printHex(const char* label, const uint8_t* data, size_t len, size_t maxBytes = 64) {
    Serial.printf("\n%s (%u bytes total):\n", label, len);
    
    size_t bytesToPrint = (len < maxBytes) ? len : maxBytes;
    
    for(size_t i = 0; i < bytesToPrint; i++) {
        Serial.printf("%02X", data[i]);
        
        if((i + 1) % 32 == 0) {
            Serial.println();
        } else if((i + 1) % 4 == 0) {
            Serial.print(" ");
        }
    }
    
    if(len > maxBytes) {
        Serial.printf("\n... (%u more bytes not shown)\n", len - maxBytes);
    }
    Serial.println("\n");
}


void dilithiumTask(void *param)
{
    Serial.println("=== Dilithium Task Started ===\n");

    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t sig[CRYPTO_BYTES];

    size_t siglen = 0;
    const char *msg = "hello world";
    size_t msglen = strlen(msg);

    Serial.printf("Message: \"%s\" (%u bytes)\n\n", msg, msglen);

    Serial.println("┌────────────────────────────────┐");
    Serial.println("│     GENERATING KEYPAIR         │");
    Serial.println("└────────────────────────────────┘");
    
    int r1 = dilithium_keypair(pk, sk);
    Serial.printf("keypair() returned %d\n", r1);
    
    if(r1 == 0) {
        Serial.println("✓ Success!");
        
        printHex("PUBLIC KEY", pk, CRYPTO_PUBLICKEYBYTES, 64);
        
        printHex("SECRET KEY", sk, CRYPTO_SECRETKEYBYTES, 64);
    } else {
        Serial.println("✗ Failed!");
        vTaskDelete(NULL);
        return;
    }

    Serial.println("┌────────────────────────────────┐");
    Serial.println("│        SIGNING MESSAGE         │");
    Serial.println("└────────────────────────────────┘");
    
    int r2 = dilithium_sign(sig, &siglen, (uint8_t*)msg, msglen, sk);
    Serial.printf("sign() returned %d, siglen=%u\n", r2, siglen);
    
    if(r2 == 0) {
        Serial.println("✓ Success!");
        
        printHex("SIGNATURE", sig, siglen, 64);
    } else {
        Serial.println("✗ Failed!");
        vTaskDelete(NULL);
        return;
    }

    Serial.println("┌────────────────────────────────┐");
    Serial.println("│      VERIFYING SIGNATURE       │");
    Serial.println("└────────────────────────────────┘");
    
    int r3 = dilithium_verify(sig, siglen, (uint8_t*)msg, msglen, pk);
    Serial.printf("verify() returned %d\n", r3);
    
    if(r3 == 0) {
        Serial.println("✓ Signature is VALID!\n");
    } else {
        Serial.println("✗ Signature is INVALID!\n");
    }

    Serial.println("┌────────────────────────────────┐");
    Serial.println("│   TESTING TAMPER DETECTION     │");
    Serial.println("└────────────────────────────────┘");
    Serial.println("Modifying signature (flipping 1 bit)...");
    
    sig[0] ^= 0x01; 
    
    int r4 = dilithium_verify(sig, siglen, (uint8_t*)msg, msglen, pk);
    Serial.printf("verify() with tampered signature returned %d\n", r4);
    
    if(r4 != 0) {
        Serial.println("✓ Tamper detected correctly!\n");
    } else {
        Serial.println("✗ WARNING: Tampered signature accepted!\n");
    }

    Serial.println("=== Dilithium Test Completed ===");

    vTaskDelete(NULL); 
}


void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("Setup started");

    xTaskCreate(
        dilithiumTask,
        "DilithiumTask",
        DILITHIUM_TASK_STACK,
        NULL,
        DILITHIUM_TASK_PRIO,
        &dilithiumTaskHandle
    );
}

void loop()
{

}