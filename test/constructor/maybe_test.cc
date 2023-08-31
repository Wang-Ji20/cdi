//===------------------------------------------===
// cdi 2023
//
// Identification: test/constrcutor/maybe_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "constructor/maybe.hh"
#include "constructor/tracker.hh"
#include "gtest/gtest.h"

// NOLINTNEXTLINE
TEST(MaybeTest, LvalueLfunctionMonadTest) {
  using namespace cdi::constructor;
  auto fadd1 = [](int input) { return Maybe<int>(input + 1); };
  auto maybe1 = Maybe<int>(1);
  auto result2 = maybe1 >= fadd1;
  ASSERT_EQ(result2.value_or(0), 2);
}

// NOLINTNEXTLINE
TEST(MaybeTest, LvalueRfunctionMonadTest) {
  using namespace cdi::constructor;
  auto maybe1 = Maybe<int>(1);
  auto result2 = maybe1 >= [](int input) { return Maybe<int>(input + 1); };
  ASSERT_EQ(result2.value_or(0), 2);
}

// NOLINTNEXTLINE
TEST(MaybeTest, RvalueLfunctionMonadTest) {
  using namespace cdi::constructor;
  auto fadd1 = [](int input) { return Maybe<int>(input + 1); };
  auto result2 = Maybe<int>(1) >= fadd1;
  ASSERT_EQ(result2.value_or(0), 2);
}

// NOLINTNEXTLINE
TEST(MaybeTest, RvalueRfunctionMonadTest) {
  using namespace cdi::constructor;
  auto result2 = Maybe<int>(1) >= [](int input) { return Maybe<int>(input + 1); };
  ASSERT_EQ(result2.value_or(0), 2);
}

// NOLINTNEXTLINE
TEST(MaybeTest, NoneTest) {
  using namespace cdi::constructor;
  auto result = Maybe<int>{none} >= [](int input) { return Maybe<int>(input + 1); };
  ASSERT_EQ(result, none);
}

// NOLINTNEXTLINE
TEST(MaybeTest, MonadTest) {
  using namespace cdi::constructor;
  auto fadd1 = [](int input) { return Maybe<int>(input + 1); };
  auto fadd2 = [](int input) { return Maybe<int>(input + 2); };
  auto fadd3 = [](int input) { return Maybe<int>(input + 3); };
  auto maybe1 = Maybe<int>(1);
  auto result7 = maybe1
             .and_then(fadd1)
             .and_then(fadd2)
             .and_then(fadd3);
  auto result7monadic = maybe1 >= [](int input) { return Maybe<int>(input + 1); }
              >= [](int input) { return Maybe<int>(input + 2); }
              >= [](int input) { return Maybe<int>(input + 3); };
  ASSERT_EQ(result7.value_or(0), 7);
  ASSERT_EQ(result7monadic.value_or(0), 7);
}
