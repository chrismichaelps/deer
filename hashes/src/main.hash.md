---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @src.main
### [Signatures]
- main(argc, argv): int (CLI dispatcher)
### [Logic]
- Loads archive via ops::load
- Dispatches: resume, add, compress [--fast|--balanced|--max], decompress
- Save after add/compress
- Default: help (shows usage)
