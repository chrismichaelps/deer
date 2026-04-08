#pragma once
#include <deer/layer.h>
#include <deer/ops/archive.h>
#include <deer/types/config.h>
#include <deer/types/storage.h>

namespace deer::layer {

/** @deer.layers.Storage.Live */
inline Layer<StorageService, config::ConfigService> make_storage_layer() {
  return Layer<StorageService, config::ConfigService>{
      [](const config::ConfigService &cfg) -> StorageService {
        const std::string path = cfg.archivePath;
        return StorageService{
            [path]() -> ArchiveState { return ops::load(path); },
            [path](const ArchiveState &state) { ops::save(state, path); }};
      }};
}

} // namespace deer::layer
