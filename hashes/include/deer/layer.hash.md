---
State_ID: BigInt(0x2)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layer
### [Signatures]
- Layer<Out, In...>: struct (factory + build())
- provide(inner, outer): Layer<Out> (unary dep resolution)
- provide(inner, la, lb): Layer<Out> (binary dep resolution)
- provide(inner, la, lb, lc): Layer<Out> (ternary dep resolution)
### [Constraints]
- RequirementsIn expressed as In... variadic pack
- RequirementsOut = Out
- Fully resolved layer has empty In... ("never")
