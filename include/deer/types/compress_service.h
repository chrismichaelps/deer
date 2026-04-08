#pragma once
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <functional>

namespace deer {

/** @deer.types.CompressService */
struct CompressService {
  /** @deer.types.CompressService.Compress */
  std::function<void(ArchiveState &, config::CompressionLevel)> compress = nullptr;
  /** @deer.types.CompressService.Decompress */
  std::function<void(ArchiveState &)> decompress = nullptr;
};

} // namespace deer
