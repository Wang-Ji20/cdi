//===------------------------------------------===
// cdi 2023
//
// Identification: include/debugging/symbolize.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_SYMBOLIZE_HH
#define CDI_DEBUGGING_SYMBOLIZE_HH

#include "constructor/maybe.hh"
#include "debugging/tracer.hh"
#include <string>

namespace cdi::debugging {

auto
Symbolize(StackFrame programCounter) noexcept
    -> constructor::Maybe<std::string>;

} // namespace cdi::debugging

#endif // CDI_DEBUGGING_SYMBOLIZE_HH
