//===------------------------------------------===
// cdi 2023
//
// Identification: test/maybe/maybe_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "maybe/maybe.hh"
#include "include/gtest/gtest.h"

TEST(maybeTest, testMonad) {
  using namespace cdi::maybe;
  auto f = [](int x) { return Maybe<int>(x + 1); };
  auto g = [](int x) { return Maybe<int>(x + 2); };
  auto h = [](int x) { return Maybe<int>(x + 3); };
  auto m = Maybe<int>(1);
  auto m1 = m.and_then(f)
             .and_then(g)
             .and_then(h);
  auto m2 = m >= [](int x) { return Maybe<int>(x + 1); }
              >= [](int x) { return Maybe<int>(x + 2); }
              >= [](int x) { return Maybe<int>(x + 3); };
  EXPECT_EQ(m1.value(), 7);
  EXPECT_EQ(m2.value(), 7);
}
