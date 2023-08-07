//===------------------------------------------===
// cdi 2023
//
// Identification: include/constructor/either.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_CONSTRUCTOR_EITHER_HH
#define CDI_CONSTRUCTOR_EITHER_HH

#include <variant>

namespace cdi::constructor {

// NOLINTBEGIN(bugprone-reserved-identifier)
template <typename A, typename B> class Either;

template <class _Tp> struct __is_either : std::false_type {};

template <class _TpA, class _TpB>
struct __is_either<Either<_TpA, _TpB>> : std::true_type {};
// NOLINTEND(bugprone-reserved-identifier)

/// \brief Either is a type that represents values with two possibilities:
/// either `Left` or `Right`.
///
/// instance Monad (Either e) where
///  Left  l >>= _ = Left l
///  Right r >>= k = k r
///
///  pure          = Right
///
/// instance Functor (Either a) where
///  fmap _ (Left x) = Left x
///  fmap f (Right y) = Right (f y)
///
template <typename A, typename B> class Either : public std::variant<A, B> {
public:
  using left_type = A;
  using right_type = B;
  using original = std::variant<A, B>;
  using original::variant;

  /// transform: alias to fmap
  /// fmap :: (a -> b) -> Either e a -> Either e b
  template <typename Func> constexpr auto transform(Func &&func) & {
    using ResTy = std::remove_cv_t<std::invoke_result_t<Func, B>>;
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable");
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return Either<A, ResTy>(func(std::get<B>(*this)));
  }

  template <typename Func> constexpr auto transform(Func &&func) const & {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable");
    using ResTy = std::remove_cv_t<std::invoke_result_t<Func, B>>;
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return Either<A, ResTy>(func(std::get<B>(*this)));
  }

  template <typename Func> constexpr auto transform(Func &&func) && {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable");
    using ResTy = std::remove_cv_t<std::invoke_result_t<Func, B>>;
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return Either<A, ResTy>(func(std::get<B>(*this)));
  }

  template <typename Func> constexpr auto transform(Func &&func) const && {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable");
    using ResTy = std::remove_cv_t<std::invoke_result_t<Func, B>>;
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return Either<A, ResTy>(func(std::get<B>(*this)));
  }

  /// and_then
  /// and_then :: Either e a -> (a -> Either e b) -> Either e b
  template <typename Func> constexpr auto and_then(Func &&func) & {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable with B");
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return std::invoke(std::forward<Func>(func),
                       original::template get<B>(*this));
  }

  template <typename Func> constexpr auto and_then(Func &&func) const & {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable with B");
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return std::invoke(std::forward<Func>(func),
                       original::template get<B>(*this));
  }

  template <typename Func> constexpr auto and_then(Func &&func) && {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable with B");
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return std::invoke(std::forward<Func>(func),
                       original::template get<B>(*this));
  }

  template <typename Func> constexpr auto and_then(Func &&func) const && {
    // static asserts
    static_assert(std::is_invocable_v<Func, B>, "Func is not invocable with B");
    // left, no change
    if (std::holds_alternative<A>(*this)) {
      return *this;
    }
    // right, apply func
    return std::invoke(std::forward<Func>(func),
                       original::template get<B>(*this));
  }
  
};

} // namespace cdi::constructor

#endif // CDI_CONSTRUCTOR_EITHER_HH
