//===------------------------------------------===
// cdi 2023
//
// Identification: test/constrcutor/maybe_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "constructor/maybe.hh"
#include "gtest/gtest.h"

TEST(ConstructorTest, MonadTest) {
  using namespace cdi::constructor;
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
  ASSERT_EQ(m1.value(), 7);
  ASSERT_EQ(m2.value(), 7);
}
