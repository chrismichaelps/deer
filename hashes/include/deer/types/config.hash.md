---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.types.config
### [Signatures]
- ARCHIVE_PATH: const char* = "deer_archive.json"
- MAX_RECENT_TURNS: size_t = 30
- CHUNK_SIZE: size_t = 8
- TARGET_SLOTS: size_t = 25
- QUANT_BITS: int = 4
- FEATURE_COUNT_FAST: size_t = 12
- FEATURE_COUNT_BALANCED: size_t = 20
- FEATURE_COUNT_MAX: size_t = 28
- QUANTIZATION_RANGE: double = 15.0
- QUANTIZATION_MIN: int = -8
- QUANTIZATION_MAX: int = 7
- COMPRESSION_METHOD: const char* = "deer_v0.5_optimized"
- CMD_RESUME: const char* = "resume"
- CMD_ADD: const char* = "add"
- CMD_COMPRESS: const char* = "compress"
- CMD_DECOMPRESS: const char* = "decompress"
- CMD_HELP: const char* = "help"
