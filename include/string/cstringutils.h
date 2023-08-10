//===------------------------------------------===
// cdi 2023
//
// Identification: include/string/cstringutils.h
//
// Author: Ji Wang <jiwangcdi@gmail.com>
//
//===------------------------------------------===

#ifndef CDI_STRING_CSTRINGUTILS_H
#define CDI_STRING_CSTRINGUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

size_t Strlen(const char *str);

struct StrtokOptions {
  const char *s;
  const char *whitespace;
  const char *delim;
  const char *quote;
  char escape;
  bool e_strings;
  bool del_quotes;
};

void StripQuotes(char *source, char quote, char escape);

char *Strtok(struct StrtokOptions options);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
