#pragma once

#include <deer/layers.h>
#include <deer/services.h>
#include <deer/tags.h>
#include <deer/types/archive.h>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>

namespace deer {

/** @deer.Runtime.Store */
class Runtime {
public:
  Runtime() = default;

  template <typename Out, typename Error> static Runtime fromLayer(DeerLayer<Out, Error> layer) {
    Runtime runtime;
    runtime.context_ = layer.build(runtime.context_);
    return runtime;
  }

  template <typename... Layers> static Runtime fromLayers(Layers... layers) {
    Runtime runtime;
    Context ctx;
    (layers.build(ctx), ...);
    runtime.context_ = ctx;
    return runtime;
  }

  template <typename Service> std::shared_ptr<Service> get() {
    if (!context_.has<Service>()) {
      throw std::runtime_error("Service not found: " + std::string(typeid(Service).name()));
    }
    return context_.get<Service>();
  }

  template <typename Service> bool has() const {
    return context_.has<Service>();
  }

  const Context &context() const {
    return context_;
  }
  Context &context() {
    return context_;
  }

private:
  Context context_;
};

/** @deer.createRuntime.Factory */
inline Runtime createRuntime() {
  auto configLayer = Layer::succeed(ConfigLive());
  auto loggerLayer = Layer::effect<Logger, std::monostate, Config>(
      [](std::shared_ptr<Config> config) -> std::shared_ptr<Logger> { return LoggerLive(config); });
  auto archiveLayer = Layer::effect<Archive, std::monostate, Config>(
      [](std::shared_ptr<Config> config) -> std::shared_ptr<Archive> {
        return ArchiveLive(config);
      });

  Context ctx;
  auto configCtx = configLayer.build(ctx);
  ctx.merge(configCtx);
  auto loggerCtx = loggerLayer.build(ctx);
  ctx.merge(loggerCtx);
  auto archiveCtx = archiveLayer.build(ctx);
  ctx.merge(archiveCtx);

  Runtime runtime;
  runtime.context() = ctx;
  return runtime;
}

/** @deer.ServiceAccessor.Template */
template <typename Service> class ServiceAccessor {
public:
  explicit ServiceAccessor(Context &ctx) : service_(ctx.get<Service>()) {}
  Service &get() {
    return *service_;
  }
  const Service &get() const {
    return *service_;
  }
  Service *operator->() {
    return service_.get();
  }
  const Service *operator->() const {
    return service_.get();
  }
  Service &operator*() {
    return *service_;
  }
  const Service &operator*() const {
    return *service_;
  }

private:
  std::shared_ptr<Service> service_;
};

/** @deer.exampleUsage */
inline void exampleUsage() {
  auto runtime = createRuntime();
  auto config = runtime.get<Config>();
  auto logger = runtime.get<Logger>();
  auto archive = runtime.get<Archive>();
  logger->info("Runtime created successfully");
  auto state = archive->load();
  archive->save(state);
  logger->info("Operations completed");
}

} // namespace deer