//===--- backtrace_test.cc - Test Backtrace ---------------------*- C++ -*-===//
// cdi 2023
//
// Identification: test/debugging/backtrace_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#include "debugging/tracer.hh"

#include "gtest/gtest.h"

// NOLINTNEXTLINE
TEST(BacktraceTest, backtraceTest) {
  using namespace cdi::debugging;
  constexpr static int kMaxDepth = 10;
  auto frames = GetStackFrame({.maxDepth = kMaxDepth, .skipFrames = 0});
  for (auto &frame : frames) {
    std::cout << frame << std::endl;
  }
}
