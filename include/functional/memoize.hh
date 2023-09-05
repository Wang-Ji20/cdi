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

// https://stackoverflow.com/questions/17805969/writing-universal-memoization-function-in-c11

template <typename OFunc, typename YFunc = OFunc>
struct Memoize;

// this template specialization is used to unpack OFunc to Ret, ...Args
template <typename Ret, typename... Args, class BaseFunc>
// direction is from the next line to above line. It unpack OFunc, and
// infer Ret, Args...
struct Memoize<Ret(Args...), BaseFunc> {
  BaseFunc func_;
  mutable std::unordered_map<std::tuple<std::decay_t<Args>...>, Ret> cache;

  template <typename U>
  Memoize(U &&func) : func_(std::forward<U>(func)) {} // NOLINT

  template <typename... Ts>
  auto
  operator()(Ts &&...args) const -> Ret {
    auto key = std::tie(args...);
    auto iter = cache.find(key);
    if (iter != cache.end()) {
      return iter->second;
    }
    auto result = func_(*this, std::forward<Ts>(args)...);
    cache.emplace(std::move(key), result);
    return result;
  }
};

// O is passed to the template in line 77
template <typename O, typename F>
auto
make_Memoize(F &&func) -> Memoize<O, std::decay_t<F>> {
  return std::forward<F>(func);
}

// Flow: O -> OFunc -> Ret, Args...

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_MEMOIZE_HH
