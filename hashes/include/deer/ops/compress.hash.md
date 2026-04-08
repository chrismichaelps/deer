---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.ops.compress
### [Signatures]
- compressArchiveWithLevel(state, level): void (v0.6: SVD/TurboQuant/Seeded)
- decompressArchive(state): void (v0.6: Inverts rotation via seed)

### [Pipeline]
- Chunking: 8 turns per chunk
- Feature Extraction: length, words, code/tool/decision + 5 rich heuristics (v0.6)
- KVTC Stage: Eigen::JacobiSVD (PCA)
- TurboQuant Stage: Seeded random orthogonal rotation (v0.6)
- Quantization: Per-channel unsigned 4-bit (0 to 15) (v0.6)
- Deflate: zlib compress
- Encoding: hex (for JSON safety)
