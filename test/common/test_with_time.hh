//===--- test_with_time.hh - Calculate elapsed time -------------*- C++ -*-===//
// cdi 2023
//
// Identification: test/common/test_with_time.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//

#ifndef CDI_TEST_COMMON_TEST_WITH_TIME_HH
#define CDI_TEST_COMMON_TEST_WITH_TIME_HH

#include <chrono>
#include <functional>
#include <iostream>
#include <string>

auto
TestWithTimeMileS(const std::function<void()>& func)
    -> std::chrono::milliseconds {
  auto start = std::chrono::high_resolution_clock::now();
  func();
  auto end = std::chrono::high_resolution_clock::now();
  auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  return elapsed;
}

#endif // CDI_TEST_COMMON_TEST_WITH_TIME_HH
