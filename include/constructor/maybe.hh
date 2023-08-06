//===------------------------------------------===
// cdi 2023
//
// Identification: include/constructor/maybe.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_CONSTRUCTOR_MAYBE_HH
#define CDI_CONSTRUCTOR_MAYBE_HH

#include "functional/functor.hh"
#include "functional/monad.hh"
#include "metaprogramming/remove.hh"

#include <optional>
#include <type_traits>

struct MAYBE_Construct_from_invoke_result_tag {
  explicit MAYBE_Construct_from_invoke_result_tag() = default;
};

namespace cdi::maybe {

// NOLINTBEGIN(bugprone-reserved-identifier)
template <typename T> class Maybe;
template <class _Tp> struct __is_maybe : std::false_type {};
template <class _Tp> struct __is_maybe<Maybe<_Tp>> : std::true_type {};
// NOLINTEND(bugprone-reserved-identifier)

/// This Maybe<T> is a Monad
///
/// Maybe T = None | Just T
///
/// fmap :: (a -> b) -> Maybe a -> Maybe b
/// fmap f (Just x) = Just (f x)
/// fmap _ None = None
///
/// pure :: a -> Maybe a
/// pure = Just
///
/// bind :: Maybe a -> (a -> Maybe b) -> Maybe b
/// bind (Just x) f = f x
/// bind None _ = None
///
template <typename T> class Maybe : public std::optional<T> {
public:
  using value_type = T;
  using original = std::optional<T>;
  using original::optional;
  using None = std::nullopt_t;

  /// TODO: Constructor & destructor
  template <class Fn, class Ux>
  constexpr Maybe(MAYBE_Construct_from_invoke_result_tag Tag,
                  Fn &&Func,
                  Ux &&Arg) noexcept(std::
                                         is_nothrow_constructible_v<
                                             value_type,
                                             std::invoke_result_t<Fn, Ux>>)
      : original(Tag, std::forward<Fn>(Func), std::forward<Ux>(Arg)) {}

  ///
  /// support of monadic operations. modified from libc++
  ///===----------------------------------------------------------------------===//
  ///
  /// Part of the LLVM Project, under the Apache License v2.0 with LLVM
  /// Exceptions. See https://llvm.org/LICENSE.txt for license information.
  /// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
  ///
  ///===----------------------------------------------------------------------===//
  ///
  ///
  // NOLINTBEGIN(bugprone-reserved-identifier)
  template <class _Func> constexpr auto and_then(_Func &&__f) & {
    using _Up = std::invoke_result_t<_Func, value_type &>;
    static_assert(
        __is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
        "Result of f(value()) must be a specialization of std::optional");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f), original::value());
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func> constexpr auto and_then(_Func &&__f) const & {
    using _Up = std::invoke_result_t<_Func, const value_type &>;
    static_assert(
        __is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
        "Result of f(value()) must be a specialization of std::optional");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f), original::value());
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func> constexpr auto and_then(_Func &&__f) && {
    using _Up = std::invoke_result_t<_Func, value_type &&>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(std::move(value())) must be a specialization of "
                  "std::optional");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f),
                         std::move(original::value()));
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func> constexpr auto and_then(_Func &&__f) const && {
    using _Up = std::invoke_result_t<_Func, const value_type &&>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(std::move(value())) must be a specialization of "
                  "std::optional");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f),
                         std::move(original::value()));
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func> constexpr auto transform(_Func &&__f) & {
    using _Up = std::remove_cv_t<std::invoke_result_t<_Func, value_type &>>;
    static_assert(!std::is_array_v<_Up>,
                  "Result of f(value()) should not be an Array");
    static_assert(!std::is_same_v<_Up, std::in_place_t>,
                  "Result of f(value()) should not be std::in_place_t");
    static_assert(!std::is_same_v<_Up, std::nullopt_t>,
                  "Result of f(value()) should not be std::nullopt_t");
    static_assert(std::is_object_v<_Up>,
                  "Result of f(value()) should be an object type");
    if (*this) {
      return original::template optional<_Up>(
          MAYBE_Construct_from_invoke_result_tag{},
          std::forward<_Func>(__f),
          original::value());
    }
    return original::template optional<_Up>();
  }

  template <class _Func> constexpr auto transform(_Func &&__f) const & {
    using _Up =
        std::remove_cv_t<std::invoke_result_t<_Func, const value_type &>>;
    static_assert(!std::is_array_v<_Up>,
                  "Result of f(value()) should not be an Array");
    static_assert(!std::is_same_v<_Up, std::in_place_t>,
                  "Result of f(value()) should not be std::in_place_t");
    static_assert(!std::is_same_v<_Up, std::nullopt_t>,
                  "Result of f(value()) should not be std::nullopt_t");
    static_assert(std::is_object_v<_Up>,
                  "Result of f(value()) should be an object type");
    if (*this) {
      return original::template optional<_Up>(
          MAYBE_Construct_from_invoke_result_tag{},
          std::forward<_Func>(__f),
          original::value());
    }
    return original::template optional<_Up>();
  }

  template <class _Func>

  constexpr auto transform(_Func &&__f) && {
    using _Up = std::remove_cv_t<std::invoke_result_t<_Func, value_type &&>>;
    static_assert(!std::is_array_v<_Up>,
                  "Result of f(std::move(value())) should not be an Array");
    static_assert(
        !std::is_same_v<_Up, std::in_place_t>,
        "Result of f(std::move(value())) should not be std::in_place_t");
    static_assert(
        !std::is_same_v<_Up, std::nullopt_t>,
        "Result of f(std::move(value())) should not be std::nullopt_t");
    static_assert(std::is_object_v<_Up>,
                  "Result of f(std::move(value())) should be an object type");
    if (*this) {
      return original::template optional<_Up>(
          MAYBE_Construct_from_invoke_result_tag{},
          std::forward<_Func>(__f),
          std::move(original::value()));
    }
    return original::template optional<_Up>();
  }

  template <class _Func> constexpr auto transform(_Func &&__f) const && {
    using _Up = cdi::metaprogramming::remove_cvref_t<
        std::invoke_result_t<_Func, const value_type &&>>;
    static_assert(!std::is_array_v<_Up>,
                  "Result of f(std::move(value())) should not be an Array");
    static_assert(
        !std::is_same_v<_Up, std::in_place_t>,
        "Result of f(std::move(value())) should not be std::in_place_t");
    static_assert(
        !std::is_same_v<_Up, std::nullopt_t>,
        "Result of f(std::move(value())) should not be std::nullopt_t");
    static_assert(std::is_object_v<_Up>,
                  "Result of f(std::move(value())) should be an object type");
    if (*this) {
      return original::template optional<_Up>(
          MAYBE_Construct_from_invoke_result_tag{},
          std::forward<_Func>(__f),
          std::move(original::value()));
    }
    return original::template optional<_Up>();
  }

  /// require is_copy_constructible_v<value_type>
  template <typename _Func>
  constexpr auto or_else(_Func &&__f) const & -> std::optional<
      std::remove_cv_t<std::invoke_result_t<_Func, value_type &&>>> {
    static_assert(std::is_invocable_v<_Func>, "f must be invocable");
    static_assert(std::is_copy_constructible_v<value_type>,
                  "value_type must be copy constructible");
    static_assert(
        std::is_same_v<
            cdi::metaprogramming::remove_cvref_t<std::invoke_result_t<_Func>>,
            typename original::optional>,
        "Result of f() should be the same type as this optional");
    if (*this) {
      return *this;
    }
    return std::forward<_Func>(__f)();
  }

  template <typename _Func>
  constexpr auto or_else(_Func &&__f) && -> std::optional<
      std::remove_cv_t<std::invoke_result_t<_Func, value_type &&>>> {
    static_assert(std::is_invocable_v<_Func>, "f must be invocable");
    static_assert(std::is_copy_constructible_v<value_type>,
                  "value_type must be copy constructible");
    static_assert(
        std::is_same_v<
            cdi::metaprogramming::remove_cvref_t<std::invoke_result_t<_Func>>,
            typename original::optional>,
        "Result of f() should be the same type as this optional");
    if (*this) {
      return *this;
    }
    return std::forward<_Func>(__f)();
  }
  // NOLINTEND(bugprone-reserved-identifier)
};

} // namespace cdi::maybe

namespace cdi::functional {

// bind, pure & fmap
template <> struct functional::Functor<cdi::maybe::Maybe> {
  template <typename Func, typename T>
  static auto fmap(Func &&mapFunc, const cdi::maybe::Maybe<T> &maybe)
      -> cdi::maybe::Maybe<std::invoke_result_t<Func, T>> {
    if (maybe) {
      return cdi::maybe::Maybe<T>(mapFunc(maybe.value()));
    }
    return cdi::maybe::Maybe<T>();
  }
};

static_assert(IsFunctor<cdi::maybe::Maybe>, "maybe is a functor");

template <> struct functional::Monad<cdi::maybe::Maybe> {
  template <typename Func, typename T>
  static auto bind(cdi::maybe::Maybe<T> maybe, Func &&bindFunc)
      -> cdi::maybe::Maybe<T> {
    if (maybe) {
      return bindFunc(maybe.value());
    }
    return cdi::maybe::Maybe<T>();
  }

  template <typename Func, typename T>
  static auto pure(T value) -> cdi::maybe::Maybe<T> {
    return cdi::maybe::Maybe<T>(value);
  }
};

static_assert(IsMonad<cdi::maybe::Maybe>, "maybe is a monad");

} // namespace cdi::functional
#endif // CDI_CONSTRUCTOR_MAYBE_HH
