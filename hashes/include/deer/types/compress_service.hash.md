---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.types.CompressService
### [Signatures]
- CompressService.compress: function<void(ArchiveState&, CompressionLevel)>
- CompressService.decompress: function<void(ArchiveState&)>
### [Constraints]
- v0.6: Uses SVD, Rotation, and Per-Channel Quant
- Eigen/zlib details hidden; interface has no deps (never)
