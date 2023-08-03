//===------------------------------------------===
// cdi 2023
//
// Identification: include/functional/functor.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// original author: 2016 Toby Allsopp
//===------------------------------------------------------------------------===

#ifndef CDI_FUNCTIONAL_FUNCTOR_HH
#define CDI_FUNCTIONAL_FUNCTOR_HH

#include <utility>

namespace cdi::functional {

/*!
    The default specialization of `Functor` is undefined; this will cause a
    compile-time error (or substitution failure when used in certain template
    contexts) if it is used.

    The first template parameter is the class template to be considered a
    Functor. The second template parameter is there to enable partial
    specializations to use SFINAE-based techniques.
   */
template <template <typename> class T, typename = void> struct Functor;

/*!
  It is very useful to have a concise way to query, at compile-time, whether a
  given class template has a valid `Functor` specialization. This is a perfect
  application of "Concepts" but as we wish to remain compatible with C++14
  compilers we have to do it the old-fashioned way, i.e. using partial
  template specialization and SFINAE.
 */
namespace detail {

/*!
  In the usual C++ template metaprogramming fashion, we define a class
  template with the default case and then use partial specialization to
  change it for specific cases. Another common idiom is the use of an extra,
  defaulted template parameter that partial specializations can use to
  SFINAE themselves away.

  The default case is that any given class template is not a Functor.
 */
template <template <typename> class, typename = void>
struct IsFunctorT : std::false_type {};

/*!
  Then we essentially say that any class template `T` is a Functor if a call to
  `Functor<T>::fmap` compiles and has the correct result type.
 */
struct dummy1 {};
struct dummy2 {};

template <template <typename> class T>
struct IsFunctorT<
    T,
    std::enable_if_t<std::is_same<T<dummy2>,
                                  decltype(Functor<T>::fmap(
                                      std::declval<dummy2(dummy1)>(),
                                      std::declval<T<dummy1>>()))>::value>>
    : std::true_type {};
} // namespace detail

/*!
  Using the `IsFunctorT` class template is little bit inconvenient, so we hide
  it away in the `detail` namespace and instead expose a `constexpr` variable
  template. An example of the use of this is coming up in the definition of
  `fmap`, below.
 */
template <template <typename> class T>
constexpr bool IsFunctor = detail::IsFunctorT<T>::value;

/*!
  To make using the `fmap` function on a Functor value more convenient, we
  define a free function that wraps it. Using this allows the Functor template
  argument to be deduced.
 */
template <template <typename> class F,
          typename A,
          typename Fun,
          typename = std::enable_if_t<IsFunctor<F>>>
auto fmap(Fun &&fun, const F<A> &functor) -> F<std::result_of_t<Fun(A)>> {
  return Functor<F>::fmap(std::forward<Fun>(fun), functor);
}

/*!
  It is useful for testing purposes to have a very basic Functor---we call
  this `NullFunctor`. It contains exactly zero values, each of the requisite
  type.
*/
namespace Test {
template <typename A> struct NullFunctor {};
} // namespace Test
template <> struct Functor<Test::NullFunctor> {
  template <typename F, typename A>
  static auto fmap(F /*unused*/, Test::NullFunctor<A> /*unused*/)
      -> Test::NullFunctor<std::result_of_t<F(A)>> {
    return {};
  }
};
static_assert(IsFunctor<Test::NullFunctor>, "NullFunctor must be a Functor");

} // namespace cdi::functional

#endif // CDI_FUNCTIONAL_FUNCTOR_HH
