#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace deer {

/** @deer.Tag.Template */
template <typename T> struct Tag;

/** @deer.TagBase.Abstract */
class TagBase {
public:
  /** @deer.TagBase.Destructor */
  virtual ~TagBase() = default;
  /** @deer.TagBase.Name */
  virtual std::string name() const = 0;
};

/** @deer.Tag.Generic */
template <typename Service> struct Tag : TagBase {
  /** @deer.Tag.ServiceType */
  using ServiceType = Service;
  /** @deer.Tag.Name */
  std::string name() const override {
    return typeid(Service).name();
  }
};

/** @deer.ConfigTag.Singleton */
struct ConfigTag : TagBase {
  /** @deer.ConfigTag.Name */
  std::string name() const override {
    return "Config";
  }
  /** @deer.ConfigTag.Instance */
  static ConfigTag &instance() {
    static ConfigTag inst;
    return inst;
  }
};

/** @deer.ArchiveTag.Singleton */
struct ArchiveTag : TagBase {
  /** @deer.ArchiveTag.Name */
  std::string name() const override {
    return "Archive";
  }
  /** @deer.ArchiveTag.Instance */
  static ArchiveTag &instance() {
    static ArchiveTag inst;
    return inst;
  }
};

/** @deer.LoggerTag.Singleton */
struct LoggerTag : TagBase {
  /** @deer.LoggerTag.Name */
  std::string name() const override {
    return "Logger";
  }
  /** @deer.LoggerTag.Instance */
  static LoggerTag &instance() {
    static LoggerTag inst;
    return inst;
  }
};

/** @deer.Context.ServiceContainer */
class Context {
public:
  /** @deer.Context.Get */
  template <typename Service> std::shared_ptr<Service> get() {
    auto key = getKey<Service>();
    auto it = services_.find(key);
    if (it == services_.end()) {
      throw std::runtime_error("Service not found in context");
    }
    return std::static_pointer_cast<Service>(it->second);
  }

  /** @deer.Context.Has */
  template <typename Service> bool has() const {
    auto key = getKey<Service>();
    return services_.find(key) != services_.end();
  }

  /** @deer.Context.Provide */
  template <typename Service> void provide(std::shared_ptr<Service> service) {
    auto key = getKey<Service>();
    services_[key] = service;
  }

  /** @deer.Context.Merge */
  Context &merge(Context &other) {
    for (auto &[key, service] : other.services_) {
      services_[key] = std::move(service);
    }
    return *this;
  }

private:
  /** @deer.Context.GetKey */
  template <typename Service> static std::string getKey() {
    return typeid(Service).name();
  }

  /** @deer.Context.Services */
  std::unordered_map<std::string, std::shared_ptr<void>> services_;
};

} // namespace deer