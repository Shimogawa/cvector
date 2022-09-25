#ifndef UINT8VEC_H
#define UINT8VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define UINT8VEC_MAXSIZE (SIZE_MAX - 100)

typedef struct uint8vec {
  uint8_t* buf;
  size_t len;
  size_t cap;
} uint8vec;

uint8vec* uint8vec_new(size_t cap) {
  uint8vec* v = newp(uint8vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(uint8_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

uint8vec* uint8vec_free(uint8vec* v) {
  free(v->buf);
  free(v);
}

int i_uint8vec_resize(uint8vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == UINT8VEC_MAXSIZE) {
      return 0;
    }
    tgtcap = UINT8VEC_MAXSIZE;
  }
  uint8_t* tmpbuf = (uint8_t*)realloc(v->buf, tgtcap * sizeof(uint8_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_uint8vec_checkbound(uint8vec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int uint8vec_append(uint8vec* v, uint8_t c) {
  if (i_uint8vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_uint8vec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int uint8vec_appendn(uint8vec* v, uint8_t* p, size_t n) {
  if (i_uint8vec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_uint8vec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(uint8_t));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static uint8_t uint8vec_get(uint8vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void uint8vec_clear(uint8vec* v) { v->len = 0; }

inline static size_t uint8vec_len(uint8vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static uint8_t* uint8vec_asarr(uint8vec* v) {
  uint8_t* buf = newn(uint8_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(uint8_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static uint8_t* uint8vec_takeownership(uint8vec* v, size_t* o_len) {
  uint8_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UINT8VEC_H
