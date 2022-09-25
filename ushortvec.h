#ifndef USHORTVEC_H
#define USHORTVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct ushortvec {
  unsigned short* buf;
  size_t len;
  size_t cap;
} ushortvec;

ushortvec* ushortvec_new(size_t cap) {
  ushortvec* v = newp(ushortvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(unsigned short, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

ushortvec* ushortvec_free(ushortvec* v) {
  free(v->buf);
  free(v);
}

int i_ushortvec_resize(ushortvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  unsigned short* tmpbuf = (unsigned short*)realloc(v->buf, tgtcap * sizeof(unsigned short));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int ushortvec_append(ushortvec* v, unsigned short c) {
  if (i_ushortvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int ushortvec_appendn(ushortvec* v, unsigned short* p, size_t n) {
  if (i_ushortvec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(unsigned short));
  return 0;
}

// returns default value (0) if out of bound.
inline static unsigned short ushortvec_get(ushortvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void ushortvec_clear(ushortvec* v) { v->len = 0; }

inline static size_t ushortvec_len(ushortvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static unsigned short* ushortvec_asarr(ushortvec* v) {
  unsigned short* buf = newn(unsigned short, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(unsigned short));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static unsigned short* ushortvec_takeownership(ushortvec* v, size_t* o_len) {
  unsigned short* buf = v->buf;
  *o_len = v->len;
  free(v);
  return buf;
}

#endif  // USHORTVEC_H
