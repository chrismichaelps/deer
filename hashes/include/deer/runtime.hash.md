---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.runtime
### [Signatures]
- Runtime: class (get<Service>, has<Service>, context)
- createRuntime(): function -> Runtime (builds Config, Logger, Archive layers)
- ServiceAccessor<Service>: class (get, operator->, operator*)
