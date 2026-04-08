#pragma once
#include <deer/types/archive.h>
#include <functional>

namespace deer {

/**
 * Archive persistence service.
 * Handles reading to and from JSON structures on disk.
 */
struct StorageService {
  /// Loads the archive from storage.
  std::function<ArchiveState()> load = nullptr;
  /// Saves the current archive state to storage.
  std::function<void(const ArchiveState &)> save = nullptr;
};

} // namespace deer
