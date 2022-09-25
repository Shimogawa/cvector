#ifndef UINT16VEC_H
#define UINT16VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define UINT16VEC_MAXSIZE (SIZE_MAX - 100)

typedef struct uint16vec {
  uint16_t* buf;
  size_t len;
  size_t cap;
} uint16vec;

uint16vec* uint16vec_new(size_t cap) {
  uint16vec* v = newp(uint16vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(uint16_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

uint16vec* uint16vec_free(uint16vec* v) {
  free(v->buf);
  free(v);
}

int i_uint16vec_resize(uint16vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == UINT16VEC_MAXSIZE) {
      return 0;
    }
    tgtcap = UINT16VEC_MAXSIZE;
  }
  uint16_t* tmpbuf = (uint16_t*)realloc(v->buf, tgtcap * sizeof(uint16_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_uint16vec_checkbound(uint16vec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int uint16vec_append(uint16vec* v, uint16_t c) {
  if (i_uint16vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_uint16vec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int uint16vec_appendn(uint16vec* v, uint16_t* p, size_t n) {
  if (i_uint16vec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_uint16vec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(uint16_t));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static uint16_t uint16vec_get(uint16vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void uint16vec_clear(uint16vec* v) { v->len = 0; }

inline static size_t uint16vec_len(uint16vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static uint16_t* uint16vec_asarr(uint16vec* v) {
  uint16_t* buf = newn(uint16_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(uint16_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static uint16_t* uint16vec_takeownership(uint16vec* v, size_t* o_len) {
  uint16_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UINT16VEC_H
