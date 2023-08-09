//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/symbolize-darwin.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// Trouble: should use unique_ptr, I don't know why
//===------------------------------------------------------------------------===

#ifndef CDI_DEBUGGING_SYMBOLIZE_DARWIN_HH
#define CDI_DEBUGGING_SYMBOLIZE_DARWIN_HH

#include "constructor/maybe.hh"
#include "control/finally.hh"
#include <execinfo.h>
#include <memory>
#include <string>

namespace cdi::debugging::detail {
using namespace constructor;
using namespace std;
using namespace cdi::control;

static auto getOneBacktraceSymbol(void **programCounter)
    -> Maybe<shared_ptr<char *>> {
  char **symbols = backtrace_symbols(programCounter, 1);
  if (symbols == nullptr) {
    return none;
  }
  std::function<void(char **)> freeFunc = [](char **symbols) { free(symbols); };
  auto symbolPtr = shared_ptr<char *>(symbols, freeFunc);
  return std::move(symbolPtr);
}

static auto stripEndAddress(string_view symbol) -> Maybe<string_view> {
  auto addrpos = symbol.find(" 0x");
  if (addrpos == string_view::npos) {
    return none;
  }
  return symbol.substr(addrpos + 1);
}

static auto stripSpaces(string_view symbol) -> Maybe<string_view> {
  auto spacepos = symbol.find(' ');
  if (spacepos == string_view::npos) {
    return none;
  }
  return symbol.substr(spacepos + 1);
}

static auto stripOffset(string_view symbol) -> Maybe<string> {
  auto pluspos = symbol.find('+');
  if (pluspos == string_view::npos) {
    return none;
  }
  return string(symbol.substr(0, pluspos));
}

// clang-format off
static auto
stripBacktraceSymbol(const shared_ptr<char *>& managedSymbols) -> Maybe<string> {
  return Maybe<string_view>(managedSymbols.get()[0])
      >= stripEndAddress
      >= stripSpaces
      >= stripOffset;
}

/// precondition: programCounter is not null, output is not null, enforced by
/// reference.
///
/// postcondition:
/// True -> output contains the symbol name of the program counter, truncated if
/// necessary.
///
/// False -> failed to symbolize the program counter, output is not modified.
auto
SymbolizeImpl(void *&programCounter) noexcept -> Maybe<string> {
  return Maybe<void **>(const_cast<void **>(&programCounter))
      >= getOneBacktraceSymbol
      >= stripBacktraceSymbol;
}

// clang-format on

} // namespace cdi::debugging::detail

#endif
