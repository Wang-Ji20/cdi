//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/tracer-gcc.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_TRACER_GCC_HH
#define CDI_DEBUGGING_TRACER_GCC_HH

#include "container/array.hh"
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

static auto UnwindDefaultImpl(void **&result,
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

  // cannot find frame size now
  std::memset(sizes, 0, sizeof(*sizes) * resultCount);
  if (min_dropped_frames != nullptr) {
    if (size - skip_count - max_depth > 0) {
      *min_dropped_frames = size - skip_count - max_depth;
    } else {
      *min_dropped_frames = 0;
    }
  }
  --recursive;
  return resultCount;
}

}; // namespace cdi::debugging::detail

#endif // CDI_DEBUGGING_TRACER_GCC_HH
