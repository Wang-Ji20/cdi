//===------------------------------------------===
// cdi 2023
//
// Identification: test/debugging/symbolize_test.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "debugging/symbolize.hh"
#include "debugging/tracer.hh"

auto functionA() -> int { return 7; }

auto functionB(int args) -> int { return args + 1 + functionA(); }

auto functionC(int args) -> int { return functionB(args) + 1; }

void functionD() {
  using namespace std;
  using namespace cdi::debugging;
  void **fps = new void *[1024]();
  int *sizes = new int[1024];
  auto fms = cdi::debugging::GetStackFrame(fps, sizes, 64, 0);
  std::cerr << fms << std::endl;
  string res;
  if (Symbolize(fps[0], res)) {
    cerr << res << endl;
  }
}

TEST(SymbolizeTest, SimpleTest) { functionD(); }
