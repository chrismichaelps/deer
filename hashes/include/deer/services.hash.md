---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.services
### [Signatures]
- Config: struct (virtual getArchivePath, getMaxRecentTurns, getTargetSlots, getQuantBits)
- Archive: struct (virtual load, save)
- Logger: struct (virtual log, info, error)
- ConfigLiveImpl: class Config
- LoggerLiveImpl: class Logger (depends on Config)
- ArchiveLiveImpl: class Archive (depends on Config)
