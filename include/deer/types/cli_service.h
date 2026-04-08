#pragma once
#include <deer/types/archive.h>
#include <functional>

namespace deer {

/** @deer.types.CliService */
struct CliService {
  /** @deer.types.CliService.Resume */
  std::function<void(const ArchiveState &)> resume = nullptr;
  /** @deer.types.CliService.Add */
  std::function<void(ArchiveState &)> add = nullptr;
  /** @deer.types.CliService.Help */
  std::function<void()> help = nullptr;
};

} // namespace deer
