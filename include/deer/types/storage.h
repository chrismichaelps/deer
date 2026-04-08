#pragma once
#include <deer/types/archive.h>
#include <functional>

namespace deer {

/** @deer.types.StorageService */
struct StorageService {
  /** @deer.types.StorageService.Load */
  std::function<ArchiveState()> load = nullptr;
  /** @deer.types.StorageService.Save */
  std::function<void(const ArchiveState &)> save = nullptr;
};

} // namespace deer
