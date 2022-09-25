#ifndef INT8VEC_H
#define INT8VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct int8vec {
  int8_t* buf;
  size_t len;
  size_t cap;
} int8vec;

int8vec* int8vec_new(size_t cap) {
  int8vec* v = newp(int8vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(int8_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

int8vec* int8vec_free(int8vec* v) {
  free(v->buf);
  free(v);
}

int i_int8vec_resize(int8vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  int8_t* tmpbuf = (int8_t*)realloc(v->buf, tgtcap * sizeof(int8_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int8vec_append(int8vec* v, int8_t c) {
  if (i_int8vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int8vec_appendn(int8vec* v, int8_t* p, size_t n) {
  if (i_int8vec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(int8_t));
  return 0;
}

// returns default value (0) if out of bound.
inline static int8_t int8vec_get(int8vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void int8vec_clear(int8vec* v) { v->len = 0; }

inline static size_t int8vec_len(int8vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static int8_t* int8vec_asarr(int8vec* v) {
  int8_t* buf = newn(int8_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(int8_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static int8_t* int8vec_takeownership(int8vec* v, size_t* o_len) {
  int8_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // INT8VEC_H
