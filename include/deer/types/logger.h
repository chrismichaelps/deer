#pragma once
#include <functional>
#include <string_view>

namespace deer {

/**
 * Internal logging service.
 * Provides a standardized way to emit debug and operational messages.
 */
struct LoggerService {
  /// Simple string-based log emission.
  std::function<void(std::string_view)> log = nullptr;
};

} // namespace deer
