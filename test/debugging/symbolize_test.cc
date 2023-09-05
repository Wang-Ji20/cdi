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
}

TEST(SymbolizeTest, SimpleTest) { functionD(); }
