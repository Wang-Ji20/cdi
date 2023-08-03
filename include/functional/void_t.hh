//===------------------------------------------===
// cdi 2023
//
// Identification: include/functional/void_t.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// original author: 2016 Toby Allsopp
//===------------------------------------------------------------------------===

#ifndef CDI_FUNCTIONAL_VOID_T_HH
#define CDI_FUNCTIONAL_VOID_T_HH

namespace cdi {

template <typename... Ts> struct make_void {
  using type = void;
};
template <typename... Ts> using void_t = typename make_void<Ts...>::type;

} // namespace cdi

#endif // CDI_FUNCTIONAL_VOID_T_HH
