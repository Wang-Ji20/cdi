//===------------------------------------------===
// cdi 2023
//
// Identification: include/container/array.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_CONTAINER_ARRAY_HH
#define CDI_CONTAINER_ARRAY_HH

#include <array>

namespace cdi::container {

template <class T, std::size_t N> using array = std::array<T, N>;

// https://en.cppreference.com/w/cpp/container/array/to_array#Version_1
namespace detail {
template <class T, std::size_t N, std::size_t... I>
// NOLINTNEXTLINE
constexpr auto to_array_impl(T (&a)[N], std::index_sequence<I...>)
    -> std::array<std::remove_cv_t<T>, N> {
  return {{a[I]...}};
}
} // namespace detail

template <class T, std::size_t N>
// NOLINTNEXTLINE
constexpr auto to_array(T (&a)[N]) -> std::array<std::remove_cv_t<T>, N> {
  return detail::to_array_impl(a, std::make_index_sequence<N>{});
}

namespace detail {
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
// NOLINTNEXTLINE
to_array_impl(T (&&a)[N], std::index_sequence<I...>) {
  return {{std::move(a[I])...}};
}
} // namespace detail

template <class T, std::size_t N>
// NOLINTNEXTLINE
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N]) {
  return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

} // namespace cdi::container

#endif // CDI_CONTAINER_ARRAY_HH
