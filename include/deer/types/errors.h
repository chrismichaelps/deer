#pragma once
#include <stdexcept>
#include <string>

namespace deer {

/**
 * Exception thrown when the archive cannot be loaded.
 */
class ArchiveLoadError : public std::runtime_error {
public:
  /**
   * Construct a new ArchiveLoadError.
   *
   * @param msg Description of the error.
   */
  explicit ArchiveLoadError(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * Exception thrown when the archive cannot be saved.
 */
class ArchiveSaveError : public std::runtime_error {
public:
  /**
   * Construct a new ArchiveSaveError.
   *
   * @param msg Description of the error.
   */
  explicit ArchiveSaveError(const std::string &msg) : std::runtime_error(msg) {}
};

} // namespace deer