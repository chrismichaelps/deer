#pragma once
#include <deer/layer.h>
#include <deer/types/config.h>
#include <deer/types/logger.h>
#include <iostream>

namespace deer::layer {

/**
 * Constructs the LoggerService using its concrete dependencies.
 */
inline Layer<LoggerService, config::ConfigService> make_logger_layer() {
  return Layer<LoggerService, config::ConfigService>{
      [](const config::ConfigService &cfg) -> LoggerService {
        const std::string level = cfg.logLevel;
        return LoggerService{
            [level](std::string_view msg) {
              std::cout << "[" << level << "] " << msg << "\n";
            }};
      }};
}

} // namespace deer::layer
