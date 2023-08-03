//===------------------------------------------===
// cdi 2023
//
// Identification: include/maybe/maybe.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_MAYBE_MAYBE_HH
#define CDI_MAYBE_MAYBE_HH

#include <type_traits>

namespace cdi::maybe {

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
template <typename T> class Maybe {
public:
  using valueTy = T;
  static_assert(!std::is_abstract<valueTy>::value,
                "Maybe<T> requires T to be a concrete type");
};

} // namespace cdi::maybe

#endif // CDI_MAYBE_MAYBE_HH
