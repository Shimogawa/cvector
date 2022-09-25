#ifndef INTVEC_H
#define INTVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define INTVEC_MAXSIZE (SIZE_MAX - 100)

typedef struct intvec {
  int* buf;
  size_t len;
  size_t cap;
} intvec;

intvec* intvec_new(size_t cap) {
  intvec* v = newp(intvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(int, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

intvec* intvec_free(intvec* v) {
  free(v->buf);
  free(v);
}

int i_intvec_resize(intvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == INTVEC_MAXSIZE) {
      return 0;
    }
    tgtcap = INTVEC_MAXSIZE;
  }
  int* tmpbuf = (int*)realloc(v->buf, tgtcap * sizeof(int));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_intvec_checkbound(intvec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int intvec_append(intvec* v, int c) {
  if (i_intvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_intvec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int intvec_appendn(intvec* v, int* p, size_t n) {
  if (i_intvec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_intvec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(int));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static int intvec_get(intvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void intvec_clear(intvec* v) { v->len = 0; }

inline static size_t intvec_len(intvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static int* intvec_asarr(intvec* v) {
  int* buf = newn(int, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(int));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static int* intvec_takeownership(intvec* v, size_t* o_len) {
  int* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // INTVEC_H
