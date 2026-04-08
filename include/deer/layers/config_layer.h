#pragma once
#include <deer/layer.h>
#include <deer/types/config.h>

namespace deer::layer {

/**
 * Constructs the ConfigService instance with default parameters.
 */
inline Layer<config::ConfigService> make_config_layer() {
  return Layer<config::ConfigService>{[]() -> config::ConfigService {
    return config::ConfigService{};
  }};
}

} // namespace deer::layer
