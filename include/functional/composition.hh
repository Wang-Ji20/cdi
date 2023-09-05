//===--- composition.hh - Composite funcs -----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: include/functional/composition.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#ifndef CDI_FUNCTIONAL_COMPOSITION_HH
#define CDI_FUNCTIONAL_COMPOSITION_HH

#include <functional>
#include <tuple>
#include <utility>

namespace cdi::functional {

template <typename... Functions>
struct Composition {
  std::tuple<Functions...> funcs;
  static constexpr auto tupleSize = sizeof ...(Functions);

  Composition(Functions &&...funcs)
      : funcs(std::forward<Functions>(funcs)...) {}

  template <typename InputArgs, typename Fn, typename... Funcs>
  constexpr auto
  Apply(InputArgs &&args, Fn &&func, Funcs &&...funcs) const {
    if constexpr (sizeof...(funcs) == 0) {
      return func(std::forward<InputArgs>(args));
    } else {
      return func(Apply(std::forward<InputArgs>(args), funcs...));
    }
  }

  template <typename InputArgs, std::size_t... Is>
  constexpr auto
  CallImpl(InputArgs &&args, std::index_sequence<Is...> idxseq) const {
    return Apply(std::forward<InputArgs>(args), std::get<Is>(funcs)...);
  }

  template <typename Args>
  constexpr auto
  operator()(Args &&args) const {
    return CallImpl(std::forward<Args>(args),
                    std::make_index_sequence<tupleSize>());
  }

};

template <typename... Functions>
constexpr auto
Compose(Functions &&...funcs) {
  return Composition<Functions...>{std::forward<Functions>(funcs)...};
}

} // namespace cdi::functional

/// Note:
/// perfect forwarding doesn't work if A is not pure deducted.
/// i.e. when I write `std::function<B(A)> &&`, it doesn't work.
/// I must use A, B
template <typename A, typename B>
constexpr auto
operator<<=(A &&funcAtoB, B &&funcBtoC) {
  return cdi::functional::Compose(std::forward<A>(funcAtoB),
                                  std::forward<B>(funcBtoC));
}

#endif // CDI_FUNCTIONAL_COMPOSITION_HH
