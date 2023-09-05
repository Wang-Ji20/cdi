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

#include <functional>
#include <optional>

namespace cdi::constructor {

using None = std::nullopt_t;
constexpr static None none = std::nullopt;

// We want to imitate stdlib style here.
// NOLINTBEGIN(bugprone-reserved-identifier)
template <typename T>
class Maybe;
template <class _Tp>
struct __is_maybe : std::false_type {};
template <class _Tp>
struct __is_maybe<Maybe<_Tp>> : std::true_type {};
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
template <typename T>
class Maybe : public std::optional<T> {
  struct MAYBE_Construct_from_invoke_result_tag {
    explicit MAYBE_Construct_from_invoke_result_tag() = default;
  };

public:
  using value_type = T;
  using original = std::optional<T>;
  using original::optional;

  template <class Fn, class... Ux>
  constexpr Maybe(
      MAYBE_Construct_from_invoke_result_tag Tag,
      Fn &&Func,
      Ux &&...Arg) noexcept(std::
                                is_nothrow_constructible_v<
                                    value_type,
                                    std::invoke_result_t<Fn, Ux...>>)
      : original(std::invoke(Func, std::forward(Arg...))) {}

  ///
  /// support of monadic operations. modified from libc++
  ///
  /// Here I explain why there are four version for one function. these four
  /// functions are actually returning type decltype(auto). the &, const &, &&,
  /// const && are qualifiers for the *this, not ret type. depending on the
  /// status of the current object we are calling different versions of them.
  ///
  /// &, const & are for lvalues. easy to understand.
  ///
  /// but when rvalue is used? like this:
  ///   a.and_then(f1).and_then(f2);
  /// after f1 is called, the result is a rvalue. so we need to use && version
  /// of and_then.
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
  template <class _Func>
  constexpr auto
  and_then(_Func &&__f) & {
    using _Up = std::invoke_result_t<_Func, value_type &>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(value()) must be a specialization of maybe");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f), original::value());
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func>
  constexpr auto
  and_then(_Func &&__f) const & {
    using _Up = std::invoke_result_t<_Func, const value_type &>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(value()) must be a specialization of maybe");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f), original::value());
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func>
  constexpr auto
  and_then(_Func &&__f) && {
    using _Up = std::invoke_result_t<_Func, value_type &&>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(std::move(value())) must be a specialization of "
                  "maybe");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f),
                         std::move(original::value()));
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func>
  constexpr auto
  and_then(_Func &&__f) const && {
    using _Up = std::invoke_result_t<_Func, const value_type &&>;
    static_assert(__is_maybe<cdi::metaprogramming::remove_cvref_t<_Up>>::value,
                  "Result of f(std::move(value())) must be a specialization of "
                  "maybe");
    if (*this) {
      return std::invoke(std::forward<_Func>(__f),
                         std::move(original::value()));
    }
    return cdi::metaprogramming::remove_cvref_t<_Up>();
  }

  template <class _Func>
  constexpr auto
  transform(_Func &&__f) & {
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

  template <class _Func>
  constexpr auto
  transform(_Func &&__f) const & {
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

  constexpr auto
  transform(_Func &&__f) && {
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

  template <class _Func>
  constexpr auto
  transform(_Func &&__f) const && {
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
  constexpr auto
  or_else(_Func &&__f) const & -> std::optional<
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
  constexpr auto
  or_else(_Func &&__f) && -> std::optional<
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

  // unwrap
  [[nodiscard]] constexpr auto
  unwrap() & -> value_type & {
    if (*this) {
      return original::value();
    }
    throw std::bad_optional_access();
  }

  [[nodiscard]] constexpr auto
  unwrap() const & -> const value_type & {
    if (*this) {
      return original::value();
    }
    throw std::bad_optional_access();
  }

  [[nodiscard]] constexpr auto
  unwrap() && -> value_type && {
    if (*this) {
      return std::move(original::value());
    }
    throw std::bad_optional_access();
  }

  [[nodiscard]] constexpr auto
  unwrap() const && -> const value_type && {
    if (*this) {
      return std::move(original::value());
    }
    throw std::bad_optional_access();
  }
};

} // namespace cdi::constructor

namespace cdi::functional {

// bind, pure & fmap
template <>
struct functional::Functor<cdi::constructor::Maybe> {
  template <typename Func, typename T>
  static auto
  fmap(Func &&mapFunc, const cdi::constructor::Maybe<T> &maybe)
      -> cdi::constructor::Maybe<std::invoke_result_t<Func, T>> {
    return maybe.transform(mapFunc);
  }
};

static_assert(IsFunctor<cdi::constructor::Maybe>, "maybe is not a functor");

template <>
struct functional::Monad<cdi::constructor::Maybe> {
  // perfect forwarding bindFunc, but alas maybe cannot...
  template <typename Func, typename T>
  static auto
  bind(cdi::constructor::Maybe<T> &&maybe, Func &&bindFunc)
      -> std::invoke_result_t<Func, T> {
    return std::move(maybe).and_then(bindFunc);
  }

  template <typename Func, typename T>
  static auto
  bind(const cdi::constructor::Maybe<T> &maybe, Func &&bindFunc)
      -> std::invoke_result_t<Func, T> {
    return maybe.and_then(bindFunc);
  }

  /// perfect forwarding!!
  template <typename T>
  static auto
  pure(T &&value) -> cdi::constructor::Maybe<T> {
    return cdi::constructor::Maybe<T>(std::forward<T>(value));
  }
};

static_assert(IsMonad<cdi::constructor::Maybe>, "maybe is not a monad");

} // namespace cdi::functional
#endif // CDI_CONSTRUCTOR_MAYBE_HH
