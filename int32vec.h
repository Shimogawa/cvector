#ifndef INT32VEC_H
#define INT32VEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define INT32VEC_MAXSIZE (SIZE_MAX - 100)

typedef struct int32vec {
  int32_t* buf;
  size_t len;
  size_t cap;
} int32vec;

int32vec* int32vec_new(size_t cap) {
  int32vec* v = newp(int32vec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(int32_t, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

int32vec* int32vec_free(int32vec* v) {
  free(v->buf);
  free(v);
}

int i_int32vec_resize(int32vec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == INT32VEC_MAXSIZE) {
      return 0;
    }
    tgtcap = INT32VEC_MAXSIZE;
  }
  int32_t* tmpbuf = (int32_t*)realloc(v->buf, tgtcap * sizeof(int32_t));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_int32vec_checkbound(int32vec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int int32vec_append(int32vec* v, int32_t c) {
  if (i_int32vec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_int32vec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int int32vec_appendn(int32vec* v, int32_t* p, size_t n) {
  if (i_int32vec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_int32vec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(int32_t));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static int32_t int32vec_get(int32vec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void int32vec_clear(int32vec* v) { v->len = 0; }

inline static size_t int32vec_len(int32vec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static int32_t* int32vec_asarr(int32vec* v) {
  int32_t* buf = newn(int32_t, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(int32_t));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static int32_t* int32vec_takeownership(int32vec* v, size_t* o_len) {
  int32_t* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // INT32VEC_H
