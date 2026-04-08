#pragma once
#include <deer/layer.h>
#include <deer/types/config.h>

namespace deer::layer {

/** @deer.layers.Config.Live */
inline Layer<config::ConfigService> make_config_layer() {
  return Layer<config::ConfigService>{[]() -> config::ConfigService {
    return config::ConfigService{};
  }};
}

} // namespace deer::layer
