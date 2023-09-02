//===------------------------------------------===
// cdi 2023
//
// Identification: include/debugging/tracer.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_TRACER_HH
#define CDI_DEBUGGING_TRACER_HH

#include "container/vector.hh"

namespace cdi::debugging {

using StackFrame = void *;

struct UnwindOptions {
  int maxDepth;
  int skipFrames;
};

[[nodiscard("you may want to know how many stack traces to print. please check "
            "that.")]] [[maybe_unused]] extern auto
GetStackFrame(UnwindOptions opt) noexcept -> vector<StackFrame>;

} // namespace cdi::debugging

#endif // CDI_DEBUGGING_TRACER_HH
