#ifndef CHARVEC_H
#define CHARVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define THE_DEFAULT 0

typedef struct charvec {
  char* buf;
  size_t len;
  size_t cap;
} charvec;

charvec* charvec_new(size_t cap) {
  charvec* v = newp(charvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(char, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

charvec* charvec_free(charvec* v) {
  free(v->buf);
  free(v);
}

int i_charvec_resize(charvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * tgtlen
  size_t tgtcap = tgtlen << 1;
  char* tmpbuf = (char*)realloc(v->buf, tgtcap * sizeof(char));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int charvec_append(charvec* v, char c) {
  if (i_charvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, otherwise 0
int charvec_appendn(charvec* v, char* p, size_t n) {
  if (i_charvec_resize(v, v->len + n)) {
    return 1;
  }
  memcpy(v->buf + v->len, p, n * sizeof(char));
  return 0;
}

// returns default value (0) if out of bound.
inline static char charvec_get(charvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void charvec_clear(charvec* v) { v->len = 0; }

inline static size_t charvec_len(charvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static char* charvec_asarr(charvec* v) {
  char* buf = newn(char, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(char));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static char* charvec_takeownership(charvec* v, size_t* o_len) {
  char* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // CHARVEC_H
