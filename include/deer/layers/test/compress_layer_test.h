#pragma once
#include <deer/layer.h>
#include <deer/types/compress_service.h>

namespace deer::layer::test {

/** @deer.layers.test.Compress */
inline Layer<CompressService> make_compress_layer_test() {
  return Layer<CompressService>{[]() -> CompressService {
    return CompressService{
        [](ArchiveState &, config::CompressionLevel) {},
        [](ArchiveState &) {}};
  }};
}

} // namespace deer::layer::test
