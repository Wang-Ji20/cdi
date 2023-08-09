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

// apple
#if defined(__APPLE__)
#define CDI_STACKUNWIND_INCLUDE "tracer-gcc.hh"

// windows
#elif defined(_WIN32) || defined(__CYGWIN__)
#define CDI_STACKUNWIND_INCLUDE "tracer-win32.hh"

// linux. TODO

#endif // defined(__APPLE__)

//===------------------------------------------------------------------------===
// choose symbolize implementation based on platform
//===------------------------------------------------------------------------===

// apple
#if defined(__APPLE__)
#define CDI_SYMBOLIZE_INCLUDE "symbolize-darwin.hh"

// windows
#elif defined(_WIN32) || defined(__CYGWIN__)
#define CDI_SYMBOLIZE_INCLUDE "symbolize-win32.hh"

// linux
#elif defined(__linux__)
#define CDI_SYMBOLIZE_INCLUDE "symbolize-linux.hh"
#endif // defined(__APPLE__)

#endif // CDI_DEBUGGING_TRACER_CONFIG_HH
