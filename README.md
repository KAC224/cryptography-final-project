# cryptography-final-project

# Overview
This project evaluates three lattice-based post-quantum cryptographic schemes on a resource-constrained
ESP32 DevKit V1. The goal is to determine whether NIST-standard PQC can be deployed on small IoT
devices without sacrificing performance, memory, network efficiency, or energy viability.

The three implementations tested:

Scheme:     

Kyber-512:            KEM                        (Session key exchange)

Light-SABER:          KEM                        (Lightweight, bandwidth-efficient key exchange)

Dilithium-II:         Signature                  (Device authentication & firmware validation)

All algorithms are compiled from PQClean and benchmarked under identical execution conditions.

# 1. Firmware (ESP32 Implementation)
Firmware consists of Arduino-based FreeRTOS tasks that execute PQC primitives with an expanded stack
allocation (45–64 kB) to support polynomial operations.

# Key Features

● Full functional implementation of Kyber, SABER & Dilithium

● 100-run averaged benchmarks per operation (KeyGen/Encaps/Decaps/Sign/Verify)

● On-device measurement using micros() (1µs resolution)

● Hardware TRNG mapped to esp_

● random() for cryptographic entropy

All schemes compiled individually to avoid memory overlap

# 2. Results Summary
Metric:           

Runtime:            SABER ≈ Kyber         (Both feasible for real-time handshakes)

Energy Use:         Kyber-512             (Most stable per operation cost)

Memory Footprint:   SABER (static)        (No dynamic heap allocation)

Signatures:         Dilithium-II          (Sign slow & large, verify fast)

Timing Safety:      Kyber & SABER         (Dilithium shows measurable leakage)

PQC is deployable on microcontrollers for key exchange today, while signatures must be used selectively
due to size & timing behavior.

# 3. Running the Project
   
# Requirements: 
● ESP32 DevKit V1

● Arduino IDE + ESP32 Core

● Serial Monitor @115200

# Steps:
1. Open algorithm folder (Kyber/SABER/Dilithium)
   
2. Flash selected .ino benchmark file
   
3. Monitor execution output via serial
   
4. Record performance/memory/energy metrics automatically printed
   
Energy estimated as:

E = I × V × t ( 240mA × 3.3V × runtime )

Our values recorded and compiled:

https://docs.google.com/spreadsheets/d/1KiR-zsGjsxFA1s0mBJkZR86d-E4Fy5VOTnpXLBamoT4/edit?usp=sharing

# How to Use:
● For handshake systems → Kyber/SABER recommended

● For signatures & identity → Dilithium-II practical but heavy

● For minimal leakage risk → Use Kyber/SABER constant-time

This repository provides a baseline for future hardware-level PQC evaluation, optimization, and
side-channel-resistant implementations.
