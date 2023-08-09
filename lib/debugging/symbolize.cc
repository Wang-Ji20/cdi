//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/symbolize.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===


#include "debugging-config.hh"
#include "debugging/symbolize.hh"

// internal headers
#ifdef CDI_SYMBOLIZE_INCLUDE
#include CDI_SYMBOLIZE_INCLUDE
#else
#error "CDI_SYMBOLIZE_INCLUDE is not defined"
#endif

namespace cdi::debugging {

auto
Symbolize(void *&programCounter, std::string &output) noexcept -> bool {
  auto result = detail::SymbolizeImpl(programCounter);
  if (!result.has_value()) {
    return false;
  }
  output = std::move(result.value());
  return true;
}

} // namespace cdi::debugging
