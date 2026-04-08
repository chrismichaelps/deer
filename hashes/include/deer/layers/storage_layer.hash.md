---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layers.Storage.Live
### [Signatures]
- make_storage_layer(): Layer<StorageService, ConfigService>
### [Layer]
- RequirementsIn: ConfigService
- RequirementsOut: StorageService
- Captures cfg.archivePath; delegates to ops::load, ops::save
