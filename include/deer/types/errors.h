#pragma once
#include <stdexcept>
#include <string>

namespace deer {

/** @deer.ArchiveLoadError.Exception */
class ArchiveLoadError : public std::runtime_error {
public:
  /** @deer.ArchiveLoadError.Constructor */
  explicit ArchiveLoadError(const std::string &msg) : std::runtime_error(msg) {}
};

/** @deer.ArchiveSaveError.Exception */
class ArchiveSaveError : public std::runtime_error {
public:
  /** @deer.ArchiveSaveError.Constructor */
  explicit ArchiveSaveError(const std::string &msg) : std::runtime_error(msg) {}
};

} // namespace deer