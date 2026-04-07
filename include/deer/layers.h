#pragma once

#include <deer/services.h>
#include <deer/tags.h>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <deer/types/errors.h>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <variant>
#include <vector>

namespace deer {

/** @deer.LayerBase.Abstract */
class LayerBase {
public:
  /** @deer.LayerBase.Destructor */
  virtual ~LayerBase() = default;
  /** @deer.LayerBase.Build */
  virtual Context build(Context &inputContext) const = 0;
  /** @deer.LayerBase.Name */
  virtual std::string name() const = 0;
};

/** @deer.DeerLayer.Forward */
template <typename Out, typename Error = std::monostate, typename... In> class DeerLayer;

/** @deer.DeerLayer.NoDeps.Specialization */
template <typename Out, typename Error> class DeerLayer<Out, Error> : public LayerBase {
public:
  /** @deer.DeerLayer.ServiceType */
  using ServiceType = Out;
  /** @deer.DeerLayer.ErrorType */
  using ErrorType = Error;
  /** @deer.DeerLayer.Constructor */
  DeerLayer(std::shared_ptr<Out> service) : service_(std::move(service)) {}
  /** @deer.DeerLayer.Build */
  Context build(Context &) const override {
    Context ctx;
    ctx.provide<Out>(service_);
    return ctx;
  }
  /** @deer.DeerLayer.Name */
  std::string name() const override {
    return "DeerLayer<" + std::string(typeid(Out).name()) + ">";
  }

private:
  /** @deer.DeerLayer.Service */
  std::shared_ptr<Out> service_;
};

/** @deer.DeerLayer.WithDeps.Specialization */
template <typename Out, typename Error, typename... In> class DeerLayer : public LayerBase {
public:
  /** @deer.DeerLayer.ServiceType */
  using ServiceType = Out;
  /** @deer.DeerLayer.ErrorType */
  using ErrorType = Error;
  /** @deer.DeerLayer.DependencyTypes */
  using DependencyTypes = std::tuple<std::shared_ptr<In>...>;
  /** @deer.DeerLayer.Builder */
  using Builder = std::function<std::shared_ptr<Out>(std::shared_ptr<In>...)>;
  /** @deer.DeerLayer.Constructor */
  DeerLayer(Builder builder) : builder_(std::move(builder)) {}

  /** @deer.DeerLayer.Build */
  Context build(Context &inputContext) const override {
    auto deps = extractDependencies(inputContext);
    auto service = std::apply(builder_, deps);
    Context ctx;
    ctx.provide<Out>(service);
    return ctx;
  }

  /** @deer.DeerLayer.Name */
  std::string name() const override {
    return "DeerLayer<" + std::string(typeid(Out).name()) + ", ...>";
  }

private:
  /** @deer.DeerLayer.ExtractDeps */
  auto extractDependencies(Context &ctx) const {
    return extractImpl(ctx, std::index_sequence_for<In...>{});
  }
  /** @deer.DeerLayer.ExtractImpl */
  template <size_t... Is> auto extractImpl(Context &ctx, std::index_sequence<Is...>) const {
    return std::make_tuple(ctx.get<In>()...);
  }
  /** @deer.DeerLayer.Builder */
  Builder builder_;
};

/** @deer.Layer.Namespace */
namespace Layer {

/** @deer.Layer.succeed.Factory */
template <typename Service> auto succeed(std::shared_ptr<Service> service) {
  return DeerLayer<Service, std::monostate>(service);
}

/** @deer.Layer.effect.Factory */
template <typename Service, typename Error, typename... In>
auto effect(typename DeerLayer<Service, Error, In...>::Builder builder) {
  return DeerLayer<Service, Error, In...>(builder);
}

/** @deer.Layer.unwrap */
template <typename Out, typename Error, typename... In>
auto unwrap(DeerLayer<DeerLayer<Out, Error, In...>, std::monostate> outerLayer) {
  return outerLayer;
}
} // namespace Layer

/** @deer.merge.Layers */
template <typename A, typename ErrorA, typename B, typename ErrorB>
auto merge(DeerLayer<A, ErrorA> a, DeerLayer<B, ErrorB> b) {
  return [a, b](Context &ctx) {
    Context result;
    auto ctxA = a.build(ctx);
    auto ctxB = b.build(ctx);
    result.merge(ctxA);
    result.merge(ctxB);
    return result;
  };
}

/** @deer.ConfigLive.Factory */
inline std::shared_ptr<Config> ConfigLive() {
  return std::make_shared<ConfigLiveImpl>();
}

/** @deer.LoggerLive.Factory */
inline std::shared_ptr<Logger> LoggerLive(std::shared_ptr<Config> config) {
  return std::make_shared<LoggerLiveImpl>(config);
}

/** @deer.ArchiveLive.Factory */
inline std::shared_ptr<Archive> ArchiveLive(std::shared_ptr<Config> config) {
  return std::make_shared<ArchiveLiveImpl>(config);
}

/** @deer.ConfigLayer.Alias */
using ConfigLayer = DeerLayer<Config, std::monostate>;

/** @deer.LoggerLayer.Alias */
using LoggerLayer = DeerLayer<Logger, std::monostate, Config>;

/** @deer.ArchiveLayer.Alias */
using ArchiveLayer = DeerLayer<Archive, std::monostate, Config>;

/** @deer.operator|.Composition */
template <typename A, typename ErrorA, typename B, typename ErrorB>
auto operator|(DeerLayer<A, ErrorA> a, DeerLayer<B, ErrorB> b) {
  return [a, b](Context &ctx) {
    Context result;
    auto ctxA = a.build(ctx);
    auto ctxB = b.build(ctx);
    result.merge(ctxA);
    result.merge(ctxB);
    return result;
  };
}

} // namespace deer