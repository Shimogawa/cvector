#ifndef UINT32VEC_H
#define UINT32VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct uint32vec {
  uint32_t* buf;
  size_t len;
  size_t cap;
} uint32vec;

uint32vec* uint32vec_new(size_t cap) {
  uint32vec* v = newp(uint32vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(uint32_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

uint32vec* uint32vec_free(uint32vec* v) {
  free(v->buf);
  free(v);
}

int i_uint32vec_resize(uint32vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  uint32_t* tmpbuf = (uint32_t*)realloc(v->buf, tgtcap * sizeof(uint32_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uint32vec_append(uint32vec* v, uint32_t c) {
  if (i_uint32vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uint32vec_appendn(uint32vec* v, uint32_t* p, size_t n) {
  if (i_uint32vec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(uint32_t));
  return 0;
}

// returns default value (0) if out of bound.
inline static uint32_t uint32vec_get(uint32vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void uint32vec_clear(uint32vec* v) { v->len = 0; }

inline static size_t uint32vec_len(uint32vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static uint32_t* uint32vec_asarr(uint32vec* v) {
  uint32_t* buf = newn(uint32_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(uint32_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static uint32_t* uint32vec_takeownership(uint32vec* v, size_t* o_len) {
  uint32_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UINT32VEC_H
