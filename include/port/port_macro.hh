//===------------------------------------------===
// cdi 2023
//
// Identification: include/port/port_macro.hh
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

//===------------------------------------------------------------------------===
// at least I want to support UNIX & win32 in amd64
//===------------------------------------------------------------------------===

#ifndef CDI_PORT_PORT_MACRO_HH
#define CDI_PORT_PORT_MACRO_HH

//===------------------------------------------------------------------------===
// export dll
//===------------------------------------------------------------------------===

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(__GNUC__) || defined(__clang__)
#define CDI_EXPORT __attribute__((dllexport))
#else
#define CDI_EXPORT __declspec(dllexport)
#endif
#else
#if __GNUC__ >= 4
#define CDI_EXPORT __attribute__((visibility("default")))
#else
#define CDI_EXPORT
#endif
#endif

#endif // CDI_PORT_PORT_MACRO_HH
