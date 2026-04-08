---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layers.Logger.Live
### [Signatures]
- make_logger_layer(): Layer<LoggerService, ConfigService>
### [Layer]
- RequirementsIn: ConfigService
- RequirementsOut: LoggerService
- Captures cfg.logLevel at construction
