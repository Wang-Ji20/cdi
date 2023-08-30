//===--- functional_test.cc - Test FP Fun -----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: test/functional/functional_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#include "functional/composition.hh"
#include "functional/identity.hh"
#include "functional/memoize.hh"
#include "functional/y_combinator.hh"

#include "gtest/gtest.h"

// NOLINTNEXTLINE
TEST(CompositionTest, CompositeThree) {
  auto funcF = [](int input) { return input + 1; };
  auto funcG = [](int input) { return input * 2; };
  auto funcH = [](int input) { return input - 1; };

  auto fog = cdi::functional::Compose(funcF, funcG);
  auto goh = cdi::functional::Compose(funcG, funcH);

  EXPECT_EQ(fog(1), funcF(funcG(1)));
  EXPECT_EQ(goh(1), funcG(funcH(1)));
}

// NOLINTNEXTLINE
TEST(CompositionTest, CompositeOne) {
  auto funF = [](int input) { return input + 1; };
  auto funG = cdi::functional::Compose(funF);
  EXPECT_EQ(funG(1), 2);
}

// NOLINTNEXTLINE
TEST(CompositionTest, CompositeOperator) {
  auto funcF = [](int input) { return input + 1; };
  auto funcG = [](int input) { return input * 2; };
  auto funcH = [](int input) { return input - 1; };

  std::function<int(int)> fog = funcF <<= funcG;
  auto goh = funcG <<= funcH;

  EXPECT_EQ(fog(1), funcF(funcG(1)));
  EXPECT_EQ(goh(1), funcG(funcH(1)));
}

// NOLINTNEXTLINE
TEST(CompositionTest, OpMixLRValue) {
  auto funcG = [](int input) { return input * 2; };

  auto fog = [](int input) { return input + 1; } <<= funcG;
  auto goh = funcG <<= [](int input) { return input - 1; };
  auto purerv =
      [](int input) { return input + 1; }
      <<= [](int input) { return input * 2; };

  EXPECT_EQ(fog(1), 3);
  EXPECT_EQ(goh(1), 0);
  EXPECT_EQ(purerv(1), 3);
}

// NOLINTNEXTLINE
TEST(FunctionalTest, Identity) {
  auto identity = cdi::functional::Identity{};
  EXPECT_EQ(identity(1), 1);
  EXPECT_EQ(identity(2), 2);
  EXPECT_EQ(identity(3), 3);
}

// test the optimization effect of memoize using fib
std::function<int(std::function<int(int)>, int)> fibFuncp =
    [](std::function<int(int)> &&fib, int n) -> int {
  if (n < 2) {
    return n;
  }
  return fib(n - 1) + fib(n - 2);
};

cdi::functional::YCombinator<int, int> fibFunc =
    cdi::functional::make_YCombinator(std::move(fibFuncp));

// NOLINTNEXTLINE
TEST(FunctionalTest, DISABLED_Memoize) {

  cdi::functional::YCombinator<int, int> memoized_fib = fibFunc;
  const std::function<int(int)> memoFib =
      cdi::functional::Memoize<int, int>{memoized_fib};

  // start timer
  auto start = std::chrono::high_resolution_clock::now();
  EXPECT_EQ(memoized_fib(0), 0);
  EXPECT_EQ(memoized_fib(1), 1);
  EXPECT_EQ(memoized_fib(2), 1);
  EXPECT_EQ(memoized_fib(3), 2);
  EXPECT_EQ(memoized_fib(4), 3);
  EXPECT_EQ(memoized_fib(5), 5);
  EXPECT_EQ(memoized_fib(6), 8);
  EXPECT_EQ(memoized_fib(7), 13);
  EXPECT_EQ(memoized_fib(8), 21);
  EXPECT_EQ(memoized_fib(9), 34);
  EXPECT_EQ(memoized_fib(10), 55);
  EXPECT_EQ(memoized_fib(11), 89);
  EXPECT_EQ(memoized_fib(12), 144);
  EXPECT_EQ(memoized_fib(13), 233);
  EXPECT_EQ(memoized_fib(14), 377);
  EXPECT_EQ(memoized_fib(15), 610);
  EXPECT_EQ(memoized_fib(16), 987);
  EXPECT_EQ(memoized_fib(17), 1597);
  EXPECT_EQ(memoized_fib(18), 2584);
  EXPECT_EQ(memoized_fib(19), 4181);
  EXPECT_EQ(memoized_fib(20), 6765);
  EXPECT_EQ(memoized_fib(21), 10946);
  EXPECT_EQ(memoized_fib(22), 17711);
  EXPECT_EQ(memoized_fib(23), 28657);
  EXPECT_EQ(memoized_fib(24), 46368);
  EXPECT_EQ(memoized_fib(25), 75025);
  EXPECT_EQ(memoized_fib(26), 121393);
  EXPECT_EQ(memoized_fib(27), 196418);
  // end timer
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "memoized_fib(28) took " << duration.count() << " milliseconds"
            << std::endl;

  // start timer
  start = std::chrono::high_resolution_clock::now();
  EXPECT_EQ(fibFunc(0), 0);
  EXPECT_EQ(fibFunc(1), 1);
  EXPECT_EQ(fibFunc(2), 1);
  EXPECT_EQ(fibFunc(3), 2);
  EXPECT_EQ(fibFunc(4), 3);
  EXPECT_EQ(fibFunc(5), 5);
  EXPECT_EQ(fibFunc(6), 8);
  EXPECT_EQ(fibFunc(7), 13);
  EXPECT_EQ(fibFunc(8), 21);
  EXPECT_EQ(fibFunc(9), 34);
  EXPECT_EQ(fibFunc(10), 55);
  EXPECT_EQ(fibFunc(11), 89);
  EXPECT_EQ(fibFunc(12), 144);
  EXPECT_EQ(fibFunc(13), 233);
  EXPECT_EQ(fibFunc(14), 377);
  EXPECT_EQ(fibFunc(15), 610);
  EXPECT_EQ(fibFunc(16), 987);
  EXPECT_EQ(fibFunc(17), 1597);
  EXPECT_EQ(fibFunc(18), 2584);
  EXPECT_EQ(fibFunc(19), 4181);
  EXPECT_EQ(fibFunc(20), 6765);
  EXPECT_EQ(fibFunc(21), 10946);
  EXPECT_EQ(fibFunc(22), 17711);
  EXPECT_EQ(fibFunc(23), 28657);
  EXPECT_EQ(fibFunc(24), 46368);
  EXPECT_EQ(fibFunc(25), 75025);
  EXPECT_EQ(fibFunc(26), 121393);
  EXPECT_EQ(fibFunc(27), 196418);
  // end timer
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "fib(28) took " << duration.count() << " milliseconds"
            << std::endl;
}
