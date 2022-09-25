#ifndef INT64VEC_H
#define INT64VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct int64vec {
  int64_t* buf;
  size_t len;
  size_t cap;
} int64vec;

int64vec* int64vec_new(size_t cap) {
  int64vec* v = newp(int64vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(int64_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

int64vec* int64vec_free(int64vec* v) {
  free(v->buf);
  free(v);
}

int i_int64vec_resize(int64vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  int64_t* tmpbuf = (int64_t*)realloc(v->buf, tgtcap * sizeof(int64_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int64vec_append(int64vec* v, int64_t c) {
  if (i_int64vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int int64vec_appendn(int64vec* v, int64_t* p, size_t n) {
  if (i_int64vec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(int64_t));
  return 0;
}

// returns default value (0) if out of bound.
inline static int64_t int64vec_get(int64vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void int64vec_clear(int64vec* v) { v->len = 0; }

inline static size_t int64vec_len(int64vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static int64_t* int64vec_asarr(int64vec* v) {
  int64_t* buf = newn(int64_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(int64_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static int64_t* int64vec_takeownership(int64vec* v, size_t* o_len) {
  int64_t* buf = v->buf;
  *o_len = v->len;
  free(v);
  return buf;
}

#endif  // INT64VEC_H
