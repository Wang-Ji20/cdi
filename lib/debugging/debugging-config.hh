//===------------------------------------------===
// cdi 2023
//
// Identification: lib/debugging/debugging-config.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_DEBUGGING_TRACER_CONFIG_HH
#define CDI_DEBUGGING_TRACER_CONFIG_HH

//===------------------------------------------------------------------------===
// choose stackunwind implementation based on platform
//===------------------------------------------------------------------------===

// libunwind
#if defined(__APPLE__) || defined(__linux__)
#define CDI_STACKUNWIND_INCLUDE "tracer-libunwind.hh"

// windows
#elif defined(_WIN32) || defined(__CYGWIN__)
#define CDI_STACKUNWIND_INCLUDE "tracer-win32.hh"

#endif // defined(__APPLE__) || defined(__linux__)

//===------------------------------------------------------------------------===
// choose symbolize implementation based on platform
//===------------------------------------------------------------------------===

// libunwind
#if defined(__APPLE__) || defined(__linux__)
#define CDI_SYMBOLIZE_INCLUDE "symbolize-libunwind.hh"

// windows
#elif defined(_WIN32) || defined(__CYGWIN__)
#define CDI_SYMBOLIZE_INCLUDE "symbolize-win32.hh"

#endif // defined(__APPLE__) || defined (__linux__)

#endif // CDI_DEBUGGING_TRACER_CONFIG_HH
