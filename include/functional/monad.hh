//===------------------------------------------===
// cdi 2023
//
// Identification: include/functional/monad.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// original author: 2016 Toby Allsopp
//===------------------------------------------------------------------------===

#ifndef CDI_FUNCTIONAL_MONAD_HH
#define CDI_FUNCTIONAL_MONAD_HH

#include "functor.hh"
#include "void_t.hh"
#include <type_traits>

namespace cdi::functional {

template <template <typename> class M, typename Enable = void>
struct Monad;

namespace {
template <template <typename> class M, typename Enable = void>
struct IsMonadT : std::false_type {};

/*!
  At the moment we just have a very basic check---is there a specialization
  of Monad for the template and is the template a Functor.
 */
struct dummy1 {};
struct dummy2 {};

template <template <typename> class M>
struct IsMonadT<M, void_t<Monad<M>, std::enable_if_t<IsFunctor<M>>>>
    : std::true_type {
  static_assert(IsFunctor<M>, "Monad must be a Functor");
  static_assert(
      std::is_same_v<decltype(Monad<M>::pure(dummy1{})), M<dummy1>>,
      "Monad::pure must return an object of the same constructor as its "
      "template argument");
  static_assert(
      std::is_same_v<decltype(Monad<M>::bind(
                         M<dummy1>{}, std::declval<M<dummy2>(dummy1)>())),
                     M<dummy2>>,
      "Monad::bind must return an object of the same constructor as its "
      "template "
      "argument");
};

} // namespace

template <template <typename> class M>
constexpr bool IsMonad = IsMonadT<M>::value;

/*!
  Now that we can tell (approximately) whether a class template is a Monad, we
  can define the Monad operations as free functions so that the template
  arguments can be deduced in at least some cases.
*/
// pure :: (Monad m) => a -> m a
template <template <typename> class M,
          typename A,
          typename = std::enable_if_t<IsMonad<M>>>
auto
pure(const A &param) -> M<A> {
  return Monad<M>::pure(param);
}

// bind :: (Monad m) => m a -> (a -> m b) -> m b
template <typename F,
          template <typename>
          class M,
          typename A,
          typename = std::enable_if_t<IsMonad<M>>>
auto bind(const M<A> &monad, F &&func) -> std::invoke_result_t<F, A> {
  return Monad<M>::bind(monad, std::forward<F>(func));
}

template <typename F,
          template <typename>
          class M,
          typename A,
          typename = std::enable_if_t<IsMonad<M>>>
auto bind(const M<A> &&monad, F &&func) -> std::invoke_result_t<F, A> {
  return Monad<M>::bind(std::move(monad), std::forward<F>(func));
}

} // namespace cdi::functional

/*!
  It is common in Haskell to use the infix (>>=) operator as a synonym for
  _bind_; this reduces the need for parentheses. We can do the same thing in
  C++, allowing us to write `m >>= [](){}` instead of `bind(m, [](){})`.
 */
template <template <typename> class M,
          typename A,
          typename F,
          typename = std::enable_if_t<cdi::functional::IsMonad<M>>>
auto operator>=(const M<A> &monad, F &&func) {
  return cdi::functional::bind(monad, std::forward<F>(func));
}

template <template <typename> class M,
          typename A,
          typename F,
          typename = std::enable_if_t<cdi::functional::IsMonad<M>>>
auto operator>=(const M<A> &&monad, F &&func) {
  return cdi::functional::bind(std::move(monad), std::forward<F>(func));
}

template <template <typename> class M,
          typename A,
          typename F,
          typename = std::enable_if_t<cdi::functional::IsMonad<M>>>
auto operator>=(M<A> &&monad, F &&func) {
  return cdi::functional::bind(std::move(monad), std::forward<F>(func));
}

/*!
  The (>>) operator is also commonly used in Haskell. It just throws away the
  result of evaluating the first argument and returns the second argument
  instead.

  ```haskell
  (>>) :: (Monad m) => m a -> m () -> m ()
  x >> y = x >>= \_ -> y
  ```
 */
template <template <typename> class M,
          typename A,
          typename B,
          typename = std::enable_if_t<cdi::functional::IsMonad<M>>>
auto
operator>>(const M<A> &monad, const M<B> &argu) -> M<B> {
  return cdi::functional::bind(monad, [=](auto &&) { return argu; });
}

#endif // CDI_FUNCTIONAL_MONAD_HH
