#ifndef UINT64VEC_H
#define UINT64VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct uint64vec {
  uint64_t* buf;
  size_t len;
  size_t cap;
} uint64vec;

uint64vec* uint64vec_new(size_t cap) {
  uint64vec* v = newp(uint64vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(uint64_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

uint64vec* uint64vec_free(uint64vec* v) {
  free(v->buf);
  free(v);
}

int i_uint64vec_resize(uint64vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  uint64_t* tmpbuf = (uint64_t*)realloc(v->buf, tgtcap * sizeof(uint64_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uint64vec_append(uint64vec* v, uint64_t c) {
  if (i_uint64vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uint64vec_appendn(uint64vec* v, uint64_t* p, size_t n) {
  if (i_uint64vec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(uint64_t));
  return 0;
}

// returns default value (0) if out of bound.
inline static uint64_t uint64vec_get(uint64vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void uint64vec_clear(uint64vec* v) { v->len = 0; }

inline static size_t uint64vec_len(uint64vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static uint64_t* uint64vec_asarr(uint64vec* v) {
  uint64_t* buf = newn(uint64_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(uint64_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static uint64_t* uint64vec_takeownership(uint64vec* v, size_t* o_len) {
  uint64_t* buf = v->buf;
  *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UINT64VEC_H
