#ifndef LONGLONGVEC_H
#define LONGLONGVEC_H

#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#define newp(type)    ((type*)malloc(sizeof(type)))
#define newn(type, n) ((type*)malloc(sizeof(type) * (n)))

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#define THE_DEFAULT 0
#define LONGLONGVEC_MAXSIZE (SIZE_MAX - 100)

typedef struct longlongvec {
  long long* buf;
  size_t len;
  size_t cap;
} longlongvec;

longlongvec* longlongvec_new(size_t cap) {
  longlongvec* v = newp(longlongvec);
  if (!v) {
    return NULL;
  }
  v->cap = cap;
  v->len = 0;
  v->buf = newn(long long, cap);
  if (!v->buf) {
    free(v);
    return NULL;
  }
  return v;
}

longlongvec* longlongvec_free(longlongvec* v) {
  free(v->buf);
  free(v);
}

int i_longlongvec_resize(longlongvec* v, size_t tgtlen) {
  if (tgtlen * 4 < v->cap * 3) {  // if tgtlen < 3/4 cap
    return 0;
  }
  // else set cap = 2 * max(tgtlen, cap)
  size_t tgtcap = MAX(tgtlen, v->cap) << 1;
  if (tgtcap < v->cap) {  // if overflow
    if (v->cap == LONGLONGVEC_MAXSIZE) {
      return 0;
    }
    tgtcap = LONGLONGVEC_MAXSIZE;
  }
  long long* tmpbuf = (long long*)realloc(v->buf, tgtcap * sizeof(long long));
  if (!tmpbuf) {
    return 1;
  }
  v->buf = tmpbuf;
  v->cap = tgtcap;
  return 0;
}

static inline int i_longlongvec_checkbound(longlongvec* v, size_t len) {
  return len >= v->cap;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int longlongvec_append(longlongvec* v, long long c) {
  if (i_longlongvec_resize(v, v->len + 1)) {  // if err
    return 1;
  }
  if (i_longlongvec_checkbound(v, v->len + 1)) {
    return 2;
  }
  v->buf[v->len++] = c;
  return 0;
}

// return 1 if fail to allocate new space for vector, 2 if no space available,
// otherwise 0
int longlongvec_appendn(longlongvec* v, long long* p, size_t n) {
  if (i_longlongvec_resize(v, v->len + n)) {
    return 1;
  }
  size_t newlen = v->len + n;
  if (i_longlongvec_checkbound(v, newlen)) {
    return 2;
  }
  memcpy(v->buf + v->len, p, n * sizeof(long long));
  v->len = newlen;
  return 0;
}

// returns default value (0) if out of bound.
inline static long long longlongvec_get(longlongvec* v, size_t idx) {
  if (idx > v->len) {
    return THE_DEFAULT;
  }
  return v->buf[idx];
}

inline static void longlongvec_clear(longlongvec* v) { v->len = 0; }

inline static size_t longlongvec_len(longlongvec* v) { return v->len; }

// returns a newly malloced array copy of the vector.
inline static long long* longlongvec_asarr(longlongvec* v) {
  long long* buf = newn(long long, v->len);
  if (!buf) {
    return NULL;
  }
  memcpy(buf, v->buf, v->len * sizeof(long long));
  return buf;
}

// returns the ref of the buffer and frees the object
inline static long long* longlongvec_takeownership(longlongvec* v, size_t* o_len) {
  long long* buf = v->buf;
  if (o_len) *o_len = v->len;
  free(v);
  return buf;
}

#endif  // LONGLONGVEC_H
