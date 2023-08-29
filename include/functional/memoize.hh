//===--- memoize.hh - Memoize fun results -----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: include/functional/memoize.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#ifndef CDI_FUNCTIONAL_MEMOIZE_HH
#define CDI_FUNCTIONAL_MEMOIZE_HH

#include "y_combinator.hh"

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace std {
namespace {
 constexpr uint32_t MagicNumber1 = 0x9e3779b9;
 constexpr uint32_t MagicNumber2 = 6;
 constexpr uint32_t MagicNumber3 = 2;

// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     https://stackoverflow.com/questions/4948780

template <class T>
inline void
hash_combine(std::size_t &seed, T const &value) {
  seed ^= hash<T>()(value) + MagicNumber1 + (seed << MagicNumber2) +
          (seed >> MagicNumber3);
}

// Recursive template code derived from Matthieu M.
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
  static void
  apply(size_t &seed, Tuple const &tuple) {
    HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
    hash_combine(seed, get<Index>(tuple));
  }
};

template <class Tuple>
struct HashValueImpl<Tuple, 0> {
  static void
  apply(size_t &seed, Tuple const &tuple) {
    hash_combine(seed, get<0>(tuple));
  }
};
} // namespace

template <typename... TT>
struct hash<std::tuple<TT...>> {
  auto
  operator()(std::tuple<TT...> const &ttt) const -> size_t {
    size_t seed = 0;
    HashValueImpl<std::tuple<TT...>>::apply(seed, ttt);
    return seed;
  }
};
} // namespace std

namespace cdi::functional {

template <typename Ret, typename... Args>
struct Memoize {
  std::function<Ret(Args...)> func;
  mutable std::unordered_map<std::tuple<Args...>, Ret> cache;

  template <typename... Ts>
  auto
  operator()(Ts &&...args) const -> Ret {
    auto key = std::make_tuple(std::forward<Ts>(args)...);
    auto iter = cache.find(key);
    if (iter != cache.end()) {
      return iter->second;
    }
    auto result = func(std::forward<Ts>(args)...);
    cache[key] = result;
    return result;
  }
};

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_MEMOIZE_HH
