---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layers.Cli.Live
### [Signatures]
- make_cli_layer(): Layer<CliService, LoggerService, StorageService, CompressService>
### [Layer]
- RequirementsIn: LoggerService | StorageService | CompressService
- RequirementsOut: CliService
- resume: outputs JSON payload for LLM
- add: reads stdin, appends turn, logs via LoggerService
- help: prints usage
