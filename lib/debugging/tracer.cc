//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/tracer.cc
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_TRACER_CC
#define CDI_DEBUGGING_TRACER_CC

#include "debugging/tracer.hh"
#include "constructor/maybe.hh"
#include "container/array.hh"
#include "control/finally.hh"
#include <atomic>
#include <execinfo.h>

namespace cdi::debugging::detail {

thread_local static bool inRecursive = false;

/// before program is ready, do not stack trace.
static std::atomic<bool> noStackTrace{true};

[[maybe_unused]] static int stacktraces_enabler = []() {
  using namespace cdi::container;
  array<void *, 1> unused_stack;
  // Force the first backtrace to happen early to get the one-time shared lib
  // loading (allocation) out of the way. After the first call it is much safer
  // to use backtrace from a signal handler if we crash somewhere later.
  backtrace(unused_stack.data(), 1);
  noStackTrace.store(false, std::memory_order_relaxed);
  return 0;
}();

[[gnu::always_inline]] inline static auto
NoTrace() -> bool {
  return inRecursive || noStackTrace.load(std::memory_order_relaxed);
}

[[gnu::always_inline]] inline static auto
findFirstEligibleFrame(UnwindOptions opt, int backtraceResult) -> int {
  return opt.skipFrames + 1;
}

static const int kStackLength = 64;

[[gnu::always_inline]] inline static auto
AdjustUnwindOpt(UnwindOptions opt, int backtraceResult) -> UnwindOptions {
  opt.skipFrames = findFirstEligibleFrame(opt, backtraceResult);
  auto maxDepthAfterSkip = opt.maxDepth - opt.skipFrames;
  auto nonNegDepth = std::max(0, maxDepthAfterSkip);
  auto cutoffDepth = std::min(nonNegDepth, kStackLength);
  opt.maxDepth = cutoffDepth;
  return opt;
}

[[gnu::always_inline]] inline static auto
UnwindWithoutRecursion(UnwindOptions opt) -> vector<StackFrame> {
  array<void *, kStackLength> stack;
  int size = backtrace(stack.data(), kStackLength);
  opt = AdjustUnwindOpt(opt, size);
  vector<StackFrame> result;
  result.reserve(opt.maxDepth);
  for (int i = 0; i < opt.maxDepth; i++) {
    result.push_back(stack[opt.skipFrames + i]);
  }
  return result;
}
} // namespace cdi::debugging::detail

namespace cdi::debugging {

using namespace detail;

[[maybe_unused]] auto
GetStackFrame(UnwindOptions opt) noexcept -> vector<StackFrame> {
  if (NoTrace()) {
    return {};
  }
  inRecursive = true;
  auto outRecursion = control::finally([]() { inRecursive = false; });
  return UnwindWithoutRecursion(opt);
}

}; // namespace cdi::debugging

#endif // CDI_DEBUGGING_TRACER_CC
