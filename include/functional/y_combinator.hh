//===--- y_combinator.hh - Y Combinator -------------------------*- C++ -*-===//
// cdi 2023
//
// Identification: include/functional/y_combinator.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#ifndef CDI_FUNCTIONAL_Y_COMBINATOR_HH
#define CDI_FUNCTIONAL_Y_COMBINATOR_HH

#include <functional>
#include <utility>

namespace cdi::functional {

template <typename Ret, typename... Args>
struct YCombinator {
  std::function<Ret(std::function<Ret(Args...)>, Args...)> f;

  constexpr auto
  operator()(Args &&...args) const noexcept -> Ret {
    return f(*this, std::forward<Args>(args)...);
  }

};

template <typename Ret, typename... Args>
auto
make_YCombinator(
    std::function<Ret(std::function<Ret(Args...)>, Args...)> func)
    -> YCombinator<Ret, Args...> {
  return {func};
}

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_Y_COMBINATOR_HH
