#pragma once
#include <deer/types/archive.h>
#include <functional>

namespace deer {

/**
 * Command Line Interface Service.
 * Defines the contract for human-interactive operations.
 */
struct CliService {
  /// Displays the current status and recent memory of the archive.
  std::function<void(const ArchiveState &)> resume = nullptr;
  /// Prompts user to add a new turn to the current archive.
  std::function<void(ArchiveState &)> add = nullptr;
  /// Displays help documentation and command usage.
  std::function<void()> help = nullptr;
};

} // namespace deer
