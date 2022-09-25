#ifndef SHORTVEC_H
#define SHORTVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define SHORTVEC_MAXSIZE (SIZE_MAX - 100)

typedef struct shortvec {
  short* buf;
  size_t len;
  size_t cap;
} shortvec;

shortvec* shortvec_new(size_t cap) {
  shortvec* v = newp(shortvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(short, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

shortvec* shortvec_free(shortvec* v) {
  free(v->buf);
  free(v);
}

int i_shortvec_resize(shortvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == SHORTVEC_MAXSIZE) {
      return 0;
    }
    tgtcap = SHORTVEC_MAXSIZE;
  }
  short* tmpbuf = (short*)realloc(v->buf, tgtcap * sizeof(short));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_shortvec_checkbound(shortvec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int shortvec_append(shortvec* v, short c) {
  if (i_shortvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_shortvec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int shortvec_appendn(shortvec* v, short* p, size_t n) {
  if (i_shortvec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_shortvec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(short));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static short shortvec_get(shortvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void shortvec_clear(shortvec* v) { v->len = 0; }

inline static size_t shortvec_len(shortvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static short* shortvec_asarr(shortvec* v) {
  short* buf = newn(short, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(short));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static short* shortvec_takeownership(shortvec* v, size_t* o_len) {
  short* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // SHORTVEC_H
