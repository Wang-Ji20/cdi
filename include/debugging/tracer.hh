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

/// @brief Get the stack frame
/// @param[out] frameResult stack frame, nonnull enforced by reference
/// @param[out] frameSizes size of stack frame, nonnull enforced by reference
/// @param maxDepth The max depth of stack frame
/// @param skipFrames The number of frames to skip
/// @return The number of frames
/// @note This function returns a list of stack pcs, which is not very useful.
/// and we use Symbolize to get more useful information from it.
[[nodiscard("you may want to know how many stack traces to print. please check "
            "that.")]] extern auto
GetStackFrame(void **&frameResult,
              int *&frameSizes,
              int maxDepth,
              int skipFrames) noexcept -> int;

struct StackFrame {
  void *frame;
  int size;
};

struct UnwindOptions {
  int maxDepth;
  int skipFrames;
};

[[nodiscard("you may want to know how many stack traces to print. please check "
            "that.")]] [[maybe_unused]] extern auto
GetStackFrame2(UnwindOptions opt) noexcept -> vector<StackFrame>;

} // namespace cdi::debugging

#endif // CDI_DEBUGGING_TRACER_HH
