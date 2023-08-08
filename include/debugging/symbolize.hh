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

#include "port/port.hh"

namespace cdi::debugging {

/// @brief Translate Program Counter to symbol names
/// @param programCounter The program counter
/// @param[out] out The output buffer
/// @param outSize The size of output buffer
/// @return Whether the symbolization is successful
auto Symbolize(const void *programCounter, char *out, int outSize) noexcept
    -> bool;

} // namespace cdi::debugging

#endif // CDI_DEBUGGING_SYMBOLIZE_HH
