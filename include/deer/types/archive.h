#pragma once
#include <cstddef>
#include <nlohmann/json.hpp>

namespace deer {

/** @deer.json.Alias */
using json = nlohmann::json;

/** @deer.ArchiveState.Struct */
struct ArchiveState {
  /** @deer.ArchiveState.RecentTurns */
  json recentTurns = json::array();
  /** @deer.ArchiveState.MemorySlots */
  json memorySlots = json::array();
  /** @deer.ArchiveState.CodedArchive */
  json codedArchive = json::object();
  /** @deer.ArchiveState.TokenEstimate */
  size_t tokenEstimate = 0;
};

} // namespace deer