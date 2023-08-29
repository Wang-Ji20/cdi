//===--- identity.hh - Identity function -----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: include/functional/identity.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#ifndef CDI_FUNCTIONAL_IDENTITY_HH
#define CDI_FUNCTIONAL_IDENTITY_HH

#include <utility>
#include <functional>

namespace cdi::functional {

struct Identity {
  template <typename T>
  constexpr auto
  operator()(T &&arg) const noexcept -> T && {
    return std::forward<T>(arg);
  }
};

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_IDENTITY_HH
