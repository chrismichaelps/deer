#pragma once
#include <functional>

namespace deer::layer {

/** @deer.layer.Layer */
template <typename Out, typename... In>
struct Layer {
  std::function<Out(const In &...)> factory = nullptr;

  [[nodiscard]] Out build(const In &...deps) const { return factory(deps...); }
};

/** @deer.layer.provide.Unary */
template <typename Out, typename Mid>
[[nodiscard]] Layer<Out> provide(Layer<Out, Mid> inner, Layer<Mid> outer) {
  return Layer<Out>{[inner, outer]() -> Out { return inner.build(outer.build()); }};
}

/** @deer.layer.provide.Binary */
template <typename Out, typename A, typename B>
[[nodiscard]] Layer<Out> provide(Layer<Out, A, B> inner, Layer<A> la, Layer<B> lb) {
  return Layer<Out>{[inner, la, lb]() -> Out { return inner.build(la.build(), lb.build()); }};
}

/** @deer.layer.provide.Ternary */
template <typename Out, typename A, typename B, typename C>
[[nodiscard]] Layer<Out> provide(Layer<Out, A, B, C> inner, Layer<A> la, Layer<B> lb,
                                 Layer<C> lc) {
  return Layer<Out>{
      [inner, la, lb, lc]() -> Out { return inner.build(la.build(), lb.build(), lc.build()); }};
}

} // namespace deer::layer
