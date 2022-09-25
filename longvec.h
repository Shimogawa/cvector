#ifndef LONGVEC_H
#define LONGVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct longvec {
  long* buf;
  size_t len;
  size_t cap;
} longvec;

longvec* longvec_new(size_t cap) {
  longvec* v = newp(longvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(long, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

longvec* longvec_free(longvec* v) {
  free(v->buf);
  free(v);
}

int i_longvec_resize(longvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  long* tmpbuf = (long*)realloc(v->buf, tgtcap * sizeof(long));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int longvec_append(longvec* v, long c) {
  if (i_longvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int longvec_appendn(longvec* v, long* p, size_t n) {
  if (i_longvec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(long));
  return 0;
}

// returns default value (0) if out of bound.
inline static long longvec_get(longvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void longvec_clear(longvec* v) { v->len = 0; }

inline static size_t longvec_len(longvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static long* longvec_asarr(longvec* v) {
  long* buf = newn(long, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(long));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static long* longvec_takeownership(longvec* v, size_t* o_len) {
  long* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // LONGVEC_H
