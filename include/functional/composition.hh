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

  Composition(Functions &&...funcs)
      : funcs(std::forward<Functions>(funcs)...) {}

  template <typename InputArgs, typename Fn, typename... Funcs>
  auto
  Apply(InputArgs &&args, Fn &&func, Funcs &&...funcs) const {
    if constexpr (sizeof...(funcs) == 0) {
      return func(std::forward<InputArgs>(args));
    } else {
      return func(Apply(std::forward<InputArgs>(args), funcs...));
    }
  }

  template <typename Args>
  auto
  operator()(Args &&args) const {
    return Apply(std::forward<Args>(args), std::get<Functions>(funcs)...);
  }

};

template <typename... Functions>
auto
Compose(Functions &&...funcs) {
  return Composition<Functions...>{std::forward<Functions>(funcs)...};
}

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_COMPOSITION_HH
