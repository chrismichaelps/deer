#pragma once
#include <deer/layer.h>
#include <deer/ops/compress.h>
#include <deer/types/compress_service.h>
#include <deer/types/config.h>

namespace deer::layer {

/** @deer.layers.Compress.Live */
inline Layer<CompressService, config::ConfigService> make_compress_layer() {
  return Layer<CompressService, config::ConfigService>{
      [](const config::ConfigService &) -> CompressService {
        return CompressService{
            [](ArchiveState &state, config::CompressionLevel level) {
              ops::compressArchiveWithLevel(state, level);
            },
            [](ArchiveState &state) { ops::decompressArchive(state); }};
      }};
}

} // namespace deer::layer
