//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/tracer-libunwind.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_TRACER_LIBUNWIND_HH
#define CDI_DEBUGGING_TRACER_LIBUNWIND_HH

#include "constructor/maybe.hh"
#include "container/array.hh"
#include "control/finally.hh"
#include "debugging/tracer.hh"
#include <atomic>
#include <execinfo.h>

namespace cdi::debugging::detail {

thread_local static int recursive = 0;

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

[[maybe_unused]] static auto
UnwindDefaultImpl(void **&result,
                  int *&sizes,
                  int max_depth,
                  int skip_count,
                  const void *ucp, /* unused, optional */
                  int *min_dropped_frames /* optional */) -> int {
  if ((recursive != 0) || noStackTrace.load(std::memory_order_relaxed)) {
    return 0;
  }
  ++recursive;
  static const int kStackLength = 64;
  using namespace cdi::container;
  array<void *, kStackLength> stack;
  int size;

  // size: The number of returned pc addresses stored in buffer.
  size = backtrace(stack.data(), kStackLength);

  // we then skip frames
  skip_count++; // skip current frame
  int resultCount = size - skip_count;
  if (resultCount < 0) {
    resultCount = 0;
  }
  if (resultCount > max_depth) {
    resultCount = max_depth;
  }
  for (int i = 0; i < resultCount; i++) {
    result[i] = stack[i + skip_count];
  }

  --recursive;
  return resultCount;
}

[[gnu::always_inline]] inline static auto
InRecursionOrForbidTrace() -> bool {
  return recursive != 0 || noStackTrace.load(std::memory_order_relaxed);
}

[[gnu::always_inline]] inline static auto
findFirstEligibleFrame(UnwindOptions opt, int backtraceResult) -> int {
  return opt.skipFrames + 1;
}

static const int kStackLength = 64;

[[gnu::always_inline]] inline static auto
AdjustUnwindOpt(UnwindOptions opt, int backtraceResult) -> UnwindOptions {
  opt.skipFrames = findFirstEligibleFrame(opt, backtraceResult);
  auto maxDepth = opt.maxDepth - opt.skipFrames;
  opt.maxDepth = std::max(0, maxDepth);
  opt.maxDepth = std::min(opt.maxDepth, kStackLength);
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
    result.push_back({stack[opt.skipFrames + i], 0});
  }
  return result;
}

[[maybe_unused]] static auto
GetStackFrame2(UnwindOptions opt) -> vector<StackFrame> {
  if (InRecursionOrForbidTrace()) {
    return {};
  }
  ++recursive;
  auto outRecursion = control::finally([]() { --recursive; });
  return UnwindWithoutRecursion(opt);
}

}; // namespace cdi::debugging::detail

#endif // CDI_DEBUGGING_TRACER_LIBUNWIND_HH