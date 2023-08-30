//===------------------------------------------===
// cdi 2023
//
// Identification: include/metaprogramming/remove.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_METAPROGRAMMING_REMOVE_HH
#define CDI_METAPROGRAMMING_REMOVE_HH

#include <type_traits>

namespace cdi::metaprogramming {

/// C++ 17 port of std::remove_cvref
/// https://en.cppreference.com/w/cpp/types/remove_cvref
template <class T> struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <class T> using remove_cvref_t = typename remove_cvref<T>::type;

} // namespace cdi::metaprogramming

#endif
