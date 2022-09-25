#ifndef FLOATVEC_H
#define FLOATVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define FLOATVEC_MAXSIZE (SIZE_MAX - 100)

typedef struct floatvec {
  float* buf;
  size_t len;
  size_t cap;
} floatvec;

floatvec* floatvec_new(size_t cap) {
  floatvec* v = newp(floatvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(float, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

floatvec* floatvec_free(floatvec* v) {
  free(v->buf);
  free(v);
}

int i_floatvec_resize(floatvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == FLOATVEC_MAXSIZE) {
      return 0;
    }
    tgtcap = FLOATVEC_MAXSIZE;
  }
  float* tmpbuf = (float*)realloc(v->buf, tgtcap * sizeof(float));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_floatvec_checkbound(floatvec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int floatvec_append(floatvec* v, float c) {
  if (i_floatvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_floatvec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int floatvec_appendn(floatvec* v, float* p, size_t n) {
  if (i_floatvec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_floatvec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(float));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static float floatvec_get(floatvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void floatvec_clear(floatvec* v) { v->len = 0; }

inline static size_t floatvec_len(floatvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static float* floatvec_asarr(floatvec* v) {
  float* buf = newn(float, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(float));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static float* floatvec_takeownership(floatvec* v, size_t* o_len) {
  float* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // FLOATVEC_H
