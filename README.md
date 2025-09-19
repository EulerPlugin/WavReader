# WavReader — Minimal WAV Header Inspector

A lean utility that parses and prints the **canonical WAV header** from a `.wav` file.  
It focuses on **container metadata** (RIFF/WAVE) and does **not** decode audio samples.

---

## What It Does

- Opens a WAV file in **binary** mode.
- Reads **RIFF/WAVE** header fields in order.
- Stores them in internal header structs.
- Prints a tidy, aligned summary using `std::format`.

> **Endianness:** WAV is **little-endian**. On x86/x64 this reads natively. Big-endian hosts would need byte-swapping.

---

## How It Works (Short)

- `std::ifstream::read()` requires a `char*` buffer.  
  For non-`char` fields (e.g., `uint32_t`, `uint16_t`) the code uses:
  ```cpp
  file.read(reinterpret_cast<char*>(&field), sizeof(field));
  ```
- Fixed-width **tag** fields stored as `std::array<char,4>` are read directly with `read()`.

---

## Parsed Header Fields (Canonical Order)

| Field           | Size/Type        | Expected / Meaning                      | Read Method        |
|-----------------|------------------|-----------------------------------------|--------------------|
| `chunkId`       | 4 chars          | `"RIFF"`                                | direct `read()`    |
| `chunkSize`     | `uint32_t` (4)   | file size − 8                           | `reinterpret_cast` |
| `format`        | 4 chars          | `"WAVE"`                                | direct `read()`    |
| `subChunk1Id`   | 4 chars          | `"fmt "`                                 | direct `read()`    |
| `subChunk1Size` | `uint32_t` (4)   | typically `16` for PCM                  | `reinterpret_cast` |
| `audioFormat`   | `uint16_t` (2)   | `1`=PCM, `3`=IEEE float                 | `reinterpret_cast` |
| `channelCount`  | `uint16_t` (2)   | `1`, `2`, …                             | `reinterpret_cast` |
| `sampleRate`    | `uint32_t` (4)   | e.g., `44100`                           | `reinterpret_cast` |
| `byteRate`      | `uint32_t` (4)   | `sampleRate * blockAlign`               | `reinterpret_cast` |
| `blockAlign`    | `uint16_t` (2)   | `channels * (bitsPerSample/8)`          | `reinterpret_cast` |
| `bitsPerSample` | `uint16_t` (2)   | `8`, `16`, `24`, `32`                   | `reinterpret_cast` |
| `subChunk2Id`   | 4 chars          | `"data"`                                | direct `read()`    |
| `subChunk2Size` | `uint32_t` (4)   | data payload size in bytes              | `reinterpret_cast` |

---

## Example Output (Shape)

```
            File Size : 123456
             chunkID : RIFF
            chunkSize : 123448
              format : WAVE
       SubChunk 1 Id : fmt 
      SubChunk 1 Size : 16
        Audio Format : 1
       Channel Count : 2
           SampleRate : 44100
            Byte Rate : 176400
          Block Align : 4
     Bits Per Sample : 16
      SubChunk 2 Id : data
      SubChunk 2 Size : 123412
```

---

## Quick Validations (Recommended)

- **Tags:** `chunkId=="RIFF"`, `format=="WAVE"`, `subChunk1Id=="fmt "`, `subChunk2Id=="data"`.
- **Formulas:**  
  - `blockAlign == channelCount * (bitsPerSample / 8)`  
  - `byteRate == sampleRate * blockAlign`
- **Sizes:**  
  - `chunkSize + 8 == fileSize_` (common in canonical files)  
  - `subChunk2Size <= fileSize_`

---

## Limitations (By Design)

- Assumes **canonical ordering** (`fmt ` immediately followed by `data`).  
  Files with extra chunks (`LIST`, `fact`, etc.) aren’t walked; add a chunk-scanner loop to handle them.
- Does not parse **extended fmt** (`subChunk1Size > 16`, e.g., `WAVE_FORMAT_EXTENSIBLE`).
- Does not read or interpret **sample data** (header only).

---

## Build & Run (Example)

**CMake**
```cmake
cmake_minimum_required(VERSION 3.16)
project(WavReaderProj CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(WavReader
    src/main.cpp
    src/WavReader.cpp
    include/WavReader.hpp
)
target_include_directories(WavReader PRIVATE include)
```

**main.cpp**
```cpp
#include "WavReader.hpp"
int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    wav::analyzeWav(argv[1]);
    return 0;
}
```

**Run**
```bash
./WavReader path/to/file.wav
```

---

## Note on `std::format`

Requires **C++20** and a standard library that implements `std::format`.  
If unavailable on your toolchain (common on older libc++), either upgrade or replace with the {fmt} library or plain `std::cout`.

---

## Public API (This Module)

```cpp
namespace wav {
  void analyzeWav(const std::string& pathToWav); // read + print

  class WavReader {
  public:
    explicit WavReader(std::filesystem::path path);
    void read();       // parse header
    void printInfo();  // formatted console dump
  };
}
```

---

## Roadmap (Low Effort → High Value)

- Implement a **chunk walker**: iteratively read `(id,size)`, skip unknown chunks until `"data"` is found.
- Parse **extended fmt** when `subChunk1Size > 16` (support `WAVE_FORMAT_EXTENSIBLE`).
- Strengthen **diagnostics** for malformed headers and derived-field mismatches.
- Optional: peek a few **sample frames** to verify PCM vs IEEE float interpretation.


