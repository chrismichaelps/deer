---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @src.main
### [Signatures]
- main(argc, argv): int (layer graph wiring + CLI dispatcher)
### [Logic]
- Builds: config → logger, storage, compress → cli (Layer graph)
- Loads archive via storage.load()
- Dispatches: resume, add, compress [--fast|--balanced|--max], decompress
- Saves via storage.save() after add/compress
- Default: cli.help()
