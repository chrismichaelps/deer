---
State_ID: BigInt(0x1)
Git_SHA: [Current]
Grammar_Lock: "@root/hashes/grammar/cpp.hash.md"
---
## @deer.layers
### [Signatures]
- DeerLayer<Out, Error>: class (build, name)
- DeerLayer<Out, Error, In...>: class (build with dependency extraction)
- Layer::succeed<Service>: function
- Layer::effect<Service, Error, In...>: function
- ConfigLive(): function -> shared_ptr<Config>
- LoggerLive(config): function -> shared_ptr<Logger>
- ArchiveLive(config): function -> shared_ptr<Archive>
- ConfigLayer: using (DeerLayer<Config, monostate>)
- LoggerLayer: using (DeerLayer<Logger, monostate, Config>)
- ArchiveLayer: using (DeerLayer<Archive, monostate, Config>)
