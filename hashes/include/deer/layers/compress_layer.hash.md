---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layers.Compress.Live
### [Signatures]
- make_compress_layer(): Layer<CompressService, ConfigService>
### [Layer]
- RequirementsIn: ConfigService
- RequirementsOut: CompressService
- Delegates to v0.6 ops (Seeded Rotation + Per-Channel Quant)
