#pragma once
#include <deer/layer.h>
#include <deer/types/storage.h>

namespace deer::layer::test {

/** @deer.layers.test.Storage */
inline Layer<StorageService> make_storage_layer_test(ArchiveState initial = {}) {
  return Layer<StorageService>{[initial]() mutable -> StorageService {
    auto store = std::make_shared<ArchiveState>(std::move(initial));
    return StorageService{
        [store]() -> ArchiveState { return *store; },
        [store](const ArchiveState &s) { *store = s; }};
  }};
}

} // namespace deer::layer::test
