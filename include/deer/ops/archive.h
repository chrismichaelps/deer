#pragma once
#include <deer/types/archive.h>
#include <deer/types/errors.h>
#include <filesystem>
#include <fstream>

namespace deer::ops {

/** @deer.ops.load */
inline ArchiveState load(const std::filesystem::path &path) {
  ArchiveState state;
  std::ifstream file(path);
  if (!file.is_open()) {
    return state;
  }
  try {
    json j = json::parse(file);
    state.recentTurns = j.value("recentTurns", json::array());
    state.memorySlots = j.value("memorySlots", json::array());
    state.codedArchive = j.value("codedArchive", json::object());
    state.tokenEstimate = j.value("tokenEstimate", 0ULL);
  } catch (const std::exception &e) {
    throw ArchiveLoadError(std::string("Failed to load archive: ") + e.what());
  }
  return state;
}

/** @deer.ops.save */
inline void save(const ArchiveState &state, const std::filesystem::path &path) {
  json j = {{"recentTurns", state.recentTurns},
            {"memorySlots", state.memorySlots},
            {"codedArchive", state.codedArchive},
            {"tokenEstimate", state.tokenEstimate}};
  std::ofstream file(path);
  if (!file) {
    throw ArchiveSaveError("Failed to save archive");
  }
  file << j.dump(2);
}

} // namespace deer::ops