#ifndef UCHARVEC_H
#define UCHARVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define UCHARVEC_MAXSIZE (SIZE_MAX - 100)

typedef struct ucharvec {
  unsigned char* buf;
  size_t len;
  size_t cap;
} ucharvec;

ucharvec* ucharvec_new(size_t cap) {
  ucharvec* v = newp(ucharvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(unsigned char, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

ucharvec* ucharvec_free(ucharvec* v) {
  free(v->buf);
  free(v);
}

int i_ucharvec_resize(ucharvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == UCHARVEC_MAXSIZE) {
      return 0;
    }
    tgtcap = UCHARVEC_MAXSIZE;
  }
  unsigned char* tmpbuf = (unsigned char*)realloc(v->buf, tgtcap * sizeof(unsigned char));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_ucharvec_checkbound(ucharvec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int ucharvec_append(ucharvec* v, unsigned char c) {
  if (i_ucharvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_ucharvec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int ucharvec_appendn(ucharvec* v, unsigned char* p, size_t n) {
  if (i_ucharvec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_ucharvec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(unsigned char));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static unsigned char ucharvec_get(ucharvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void ucharvec_clear(ucharvec* v) { v->len = 0; }

inline static size_t ucharvec_len(ucharvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static unsigned char* ucharvec_asarr(ucharvec* v) {
  unsigned char* buf = newn(unsigned char, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(unsigned char));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static unsigned char* ucharvec_takeownership(ucharvec* v, size_t* o_len) {
  unsigned char* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UCHARVEC_H
