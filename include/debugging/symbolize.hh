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
#include <string>

namespace cdi::debugging {

/// @brief Translate Program Counter to symbol names
/// @param programCounter The program counter, nonnull
/// @param[out] output The output buffer nonnull
/// @return Whether the symbolization is successful
auto
Symbolize(void *&programCounter, std::string &output) noexcept -> bool;

} // namespace cdi::debugging

#endif // CDI_DEBUGGING_SYMBOLIZE_HH
