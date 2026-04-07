---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.ops.compress
### [Signatures]
- compressArchive(state): void (uses DEFAULT_COMPRESSION_LEVEL)
- compressArchiveWithLevel(state, level): void (KVTC/TurboQuant encoding)
- decompressArchive(state): void (decodes __deflate to feature matrix)

### [Pipeline]
- Chunking: 8 turns per chunk
- Feature Extraction: length, words, code/tool/decision detection, diversity
- KVTC Stage: Eigen::JacobiSVD (PCA)
- TurboQuant Stage: random orthogonal rotation
- Quantization: 4-bit (range -8 to +7)
- Deflate: zlib compress
- Encoding: hex (for JSON safety)
