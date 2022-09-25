#ifndef INT16VEC_H
#define INT16VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct int16vec {
  int16_t* buf;
  size_t len;
  size_t cap;
} int16vec;

int16vec* int16vec_new(size_t cap) {
  int16vec* v = newp(int16vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(int16_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

int16vec* int16vec_free(int16vec* v) {
  free(v->buf);
  free(v);
}

int i_int16vec_resize(int16vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  int16_t* tmpbuf = (int16_t*)realloc(v->buf, tgtcap * sizeof(int16_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int16vec_append(int16vec* v, int16_t c) {
  if (i_int16vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int16vec_appendn(int16vec* v, int16_t* p, size_t n) {
  if (i_int16vec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(int16_t));
  return 0;
}

// returns default value (0) if out of bound.
inline static int16_t int16vec_get(int16vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void int16vec_clear(int16vec* v) { v->len = 0; }

inline static size_t int16vec_len(int16vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static int16_t* int16vec_asarr(int16vec* v) {
  int16_t* buf = newn(int16_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(int16_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static int16_t* int16vec_takeownership(int16vec* v, size_t* o_len) {
  int16_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // INT16VEC_H
