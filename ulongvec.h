#ifndef ULONGVEC_H
#define ULONGVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct ulongvec {
  unsigned long* buf;
  size_t len;
  size_t cap;
} ulongvec;

ulongvec* ulongvec_new(size_t cap) {
  ulongvec* v = newp(ulongvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(unsigned long, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

ulongvec* ulongvec_free(ulongvec* v) {
  free(v->buf);
  free(v);
}

int i_ulongvec_resize(ulongvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  unsigned long* tmpbuf = (unsigned long*)realloc(v->buf, tgtcap * sizeof(unsigned long));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int ulongvec_append(ulongvec* v, unsigned long c) {
  if (i_ulongvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int ulongvec_appendn(ulongvec* v, unsigned long* p, size_t n) {
  if (i_ulongvec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(unsigned long));
  return 0;
}

// returns default value (0) if out of bound.
inline static unsigned long ulongvec_get(ulongvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void ulongvec_clear(ulongvec* v) { v->len = 0; }

inline static size_t ulongvec_len(ulongvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static unsigned long* ulongvec_asarr(ulongvec* v) {
  unsigned long* buf = newn(unsigned long, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(unsigned long));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static unsigned long* ulongvec_takeownership(ulongvec* v, size_t* o_len) {
  unsigned long* buf = v->buf;
  *o_len = v->len;
  free(v);
  return buf;
}

#endif  // ULONGVEC_H
