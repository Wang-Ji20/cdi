//===--- libunwind_api_test.cc - Test API -----------------------*- C++ -*-===//
// cdi 2023
//
// Identification: test/debugging/libunwind_api_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include <execinfo.h>

void someReflectiveFunction() {
  constexpr static int kBufferSize = 100;
  void *buffer[kBufferSize]; // NOLINT
  char **strings;
  int nptrs;

  nptrs = backtrace(buffer, kBufferSize);
  strings = backtrace_symbols(buffer, nptrs);
  ASSERT_TRUE(strings);
  for (int j = 0; j < nptrs; j++) {
    printf("%s\n", strings[j]);
  }
  free(strings);
}

void world() {
  someReflectiveFunction();
}

void hello() {
  world();
}

// NOLINTNEXTLINE
TEST(LibUnwindTest, backtraceTest) {
  someReflectiveFunction();
}
