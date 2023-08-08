//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/tracer.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#include "debugging/tracer.hh"

#include <atomic>

// internal headers
#include "tracer-config.hh"
#ifdef CDI_STACKUNWIND_INCLUDE
#include CDI_STACKUNWIND_INCLUDE
#else
#error "CDI_STACKUNWIND_INCLUDE is not defined"
#endif

namespace cdi::debugging {

[[gnu::always_inline]] inline auto GetStackFrame(void **&frameResult,
                                                 int *&frameSizes,
                                                 int maxDepth,
                                                 int skipFrames) noexcept
    -> int {
  return detail::UnwindDefaultImpl<true, false>(
      frameResult, frameSizes, maxDepth, skipFrames + 1, nullptr, nullptr);
}

} // namespace cdi::debugging
