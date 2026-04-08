#pragma once
#include <cstddef>
#include <nlohmann/json.hpp>

namespace deer {

/// Alias for nlohmann::json.
using json = nlohmann::json;

/**
 * Represents the state of the conversation archive.
 * Contains recent turns, memory slots, and the coded, compressed archive.
 */
struct ArchiveState {
  /// Recent uncompressed conversation turns.
  json recentTurns = json::array();
  /// User-defined memory slots.
  json memorySlots = json::array();
  /// The compressed payload of the long-term archive.
  json codedArchive = json::object();
  /// Heuristic-based count of tokens currently in memory.
  size_t tokenEstimate = 0;
};

} // namespace deer