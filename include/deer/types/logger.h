#pragma once
#include <functional>
#include <string_view>

namespace deer {

/** @deer.types.LoggerService */
struct LoggerService {
  /** @deer.types.LoggerService.Log */
  std::function<void(std::string_view)> log = nullptr;
};

} // namespace deer
