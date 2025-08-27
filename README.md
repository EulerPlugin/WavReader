# WavReader — Minimal WAV Header Inspector

A lean utility that parses and prints the **canonical WAV header** from a `.wav` file.  
It focuses on **container metadata** (RIFF/WAVE) and does **not** decode audio samples.

---

## What It Does

- Opens a WAV file in **binary** mode.
- Reads the **RIFF/WAVE** header fields in order.
- Stores them into internal header structs.
- Prints a tidy, aligned summary using `std::format`.

> **Endianness:** WAV is **little-endian**. On x86/x64 this reads natively. Big-endian hosts require byte swapping.

---

