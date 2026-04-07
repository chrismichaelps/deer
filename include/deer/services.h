#pragma once

#include <deer/tags.h>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <deer/types/errors.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace deer {

/** @deer.Config.Interface */
struct Config {
  /** @deer.Config.Destructor */
  virtual ~Config() = default;
  /** @deer.Config.GetArchivePath */
  virtual std::filesystem::path getArchivePath() const = 0;
  /** @deer.Config.GetMaxRecentTurns */
  virtual size_t getMaxRecentTurns() const = 0;
  /** @deer.Config.GetTargetSlots */
  virtual size_t getTargetSlots() const = 0;
  /** @deer.Config.GetQuantBits */
  virtual int getQuantBits() const = 0;
};

/** @deer.Archive.Interface */
struct Archive {
  /** @deer.Archive.Destructor */
  virtual ~Archive() = default;
  /** @deer.Archive.Load */
  virtual ArchiveState load() = 0;
  /** @deer.Archive.Save */
  virtual void save(const ArchiveState &state) = 0;
};

/** @deer.Logger.Interface */
struct Logger {
  /** @deer.Logger.Destructor */
  virtual ~Logger() = default;
  /** @deer.Logger.Log */
  virtual void log(const std::string &message) = 0;
  /** @deer.Logger.Info */
  virtual void info(const std::string &message) = 0;
  /** @deer.Logger.Error */
  virtual void error(const std::string &message) = 0;
};

/** @deer.ConfigLive.Implementation */
class ConfigLiveImpl : public Config {
public:
  /** @deer.ConfigLive.GetArchivePath */
  std::filesystem::path getArchivePath() const override {
    return std::filesystem::path(config::ARCHIVE_PATH);
  }
  /** @deer.ConfigLive.GetMaxRecentTurns */
  size_t getMaxRecentTurns() const override {
    return config::MAX_RECENT_TURNS;
  }
  /** @deer.ConfigLive.GetTargetSlots */
  size_t getTargetSlots() const override {
    return config::TARGET_SLOTS;
  }
  /** @deer.ConfigLive.GetQuantBits */
  int getQuantBits() const override {
    return config::QUANT_BITS;
  }
};

/** @deer.LoggerLive.Implementation */
class LoggerLiveImpl : public Logger {
public:
  /** @deer.LoggerLive.Constructor */
  explicit LoggerLiveImpl(std::shared_ptr<Config> config) : config_(std::move(config)) {}
  /** @deer.LoggerLive.Log */
  void log(const std::string &message) override {
    std::cout << "[Logger] " << message << "\n";
  }
  /** @deer.LoggerLive.Info */
  void info(const std::string &message) override {
    std::cout << "[Logger] INFO: " << message << "\n";
  }
  /** @deer.LoggerLive.Error */
  void error(const std::string &message) override {
    std::cerr << "[Logger] ERROR: " << message << "\n";
  }

private:
  /** @deer.LoggerLive.Config */
  std::shared_ptr<Config> config_;
};

/** @deer.ArchiveLive.Implementation */
class ArchiveLiveImpl : public Archive {
public:
  /** @deer.ArchiveLive.Constructor */
  explicit ArchiveLiveImpl(std::shared_ptr<Config> config) : config_(std::move(config)) {}

  /** @deer.ArchiveLive.Load */
  ArchiveState load() override {
    ArchiveState state;
    const auto path = config_->getArchivePath();
    if (!std::filesystem::exists(path)) {
      return state;
    }
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

  /** @deer.ArchiveLive.Save */
  void save(const ArchiveState &state) override {
    const auto path = config_->getArchivePath();
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

private:
  /** @deer.ArchiveLive.Config */
  std::shared_ptr<Config> config_;
};

} // namespace deer