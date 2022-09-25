#ifndef DOUBLEVEC_H
#define DOUBLEVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct doublevec {
  double* buf;
  size_t len;
  size_t cap;
} doublevec;

doublevec* doublevec_new(size_t cap) {
  doublevec* v = newp(doublevec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(double, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

doublevec* doublevec_free(doublevec* v) {
  free(v->buf);
  free(v);
}

int i_doublevec_resize(doublevec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  double* tmpbuf = (double*)realloc(v->buf, tgtcap * sizeof(double));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int doublevec_append(doublevec* v, double c) {
  if (i_doublevec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int doublevec_appendn(doublevec* v, double* p, size_t n) {
  if (i_doublevec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(double));
  return 0;
}

// returns default value (0) if out of bound.
inline static double doublevec_get(doublevec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void doublevec_clear(doublevec* v) { v->len = 0; }

inline static size_t doublevec_len(doublevec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static double* doublevec_asarr(doublevec* v) {
  double* buf = newn(double, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(double));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static double* doublevec_takeownership(doublevec* v, size_t* o_len) {
  double* buf = v->buf;
  *o_len = v->len;
  free(v);
  return buf;
}

#endif  // DOUBLEVEC_H
