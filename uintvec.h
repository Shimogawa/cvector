#ifndef UINTVEC_H
#define UINTVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct uintvec {
  unsigned int* buf;
  size_t len;
  size_t cap;
} uintvec;

uintvec* uintvec_new(size_t cap) {
  uintvec* v = newp(uintvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(unsigned int, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

uintvec* uintvec_free(uintvec* v) {
  free(v->buf);
  free(v);
}

int i_uintvec_resize(uintvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  unsigned int* tmpbuf = (unsigned int*)realloc(v->buf, tgtcap * sizeof(unsigned int));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uintvec_append(uintvec* v, unsigned int c) {
  if (i_uintvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int uintvec_appendn(uintvec* v, unsigned int* p, size_t n) {
  if (i_uintvec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(unsigned int));
  return 0;
}

// returns default value (0) if out of bound.
inline static unsigned int uintvec_get(uintvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void uintvec_clear(uintvec* v) { v->len = 0; }

inline static size_t uintvec_len(uintvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static unsigned int* uintvec_asarr(uintvec* v) {
  unsigned int* buf = newn(unsigned int, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(unsigned int));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static unsigned int* uintvec_takeownership(uintvec* v, size_t* o_len) {
  unsigned int* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // UINTVEC_H
