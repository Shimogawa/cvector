#ifndef CHARVEC_EXT_H
#define CHARVEC_EXT_H

#include "charvec.h"

// creates a newly allocated NULL terminated cstring.
static inline char* charvec_tostr(charvec* v) {
  char* s = newn(char, v->len + 1);
  if (!s) {
    return NULL;
  }
  memcpy(s, v->buf, v->len * sizeof(char));
  s[v->len] = 0;
  return s;
}

#endif  // CHARVEC_EXT_H
