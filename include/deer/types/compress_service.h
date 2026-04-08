#pragma once
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <functional>

namespace deer {

/**
 * Key-Value Tensor Compression (KVTC) Service.
 * Provides high-level operations for reducing conversation dimensionality
 * and restoring state.
 */
struct CompressService {
  /// Compresses the archive's recent turns using the specified level.
  std::function<void(ArchiveState &, config::CompressionLevel)> compress = nullptr;
  /// Decompresses the stored metadata back into a feature matrix.
  std::function<void(ArchiveState &)> decompress = nullptr;
};

} // namespace deer
