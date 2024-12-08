//
// buffer.c
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include "strbuf.h"

// TODO: shared with reference counting
// TODO: linked list for append/prepend etc

/*
 * Compute the nearest multiple of `a` from `b`.
 */

#define nearest_multiple_of(a, b) \
  (((b) + ((a) - 1)) & ~((a) - 1))

/*
 * Allocate a new buffer with BUFFER_DEFAULT_SIZE.
 */

strbuf_t * strbuf_new() {
  return strbuf_new_with_size(STRBUF_DEFAULT_SIZE);
}

/*
 * Allocate a new buffer with `n` bytes.
 */

strbuf_t * strbuf_new_with_size(size_t n) {
  strbuf_t *self = malloc(sizeof(strbuf_t));
  if (!self) return NULL;
  self->len = n;
  self->data = self->alloc = calloc(n + 1, 1);
  if (self->alloc) return self;
  free(self);
  return NULL;
}

/*
 * Allocate a new buffer with `str`.
 */

strbuf_t * strbuf_new_with_string(const char *str) {
  if (!str) return NULL;
  return strbuf_new_with_string_length(str, strlen(str));
}

/*
 * Allocate a new buffer with `str` and `len`.
 */

strbuf_t * strbuf_new_with_string_length(const char *str, size_t len) {
  strbuf_t *self = NULL;
  if (len == 0) return NULL;
  self = strbuf_new_with_size(len);
  if (!self) return NULL;
  if (memcpy(self->alloc, str, len)) {
    return self;
  }
  strbuf_free(self);
  return NULL;
}

/*
 * Allocate a new buffer with a copy of `str`.
 */

strbuf_t * strbuf_new_with_copy(const char *str) {
  size_t len = 0;
  strbuf_t *self = NULL;
  if (!str || (len = strlen(str)) == 0) return NULL;
  self = strbuf_new_with_size(len);
  if (!self) return NULL;
  if (memcpy(self->alloc, str, len)) {
    self->data = self->alloc;
    return self;
  }
  strbuf_free(self);
  return NULL;
}

/*
 * Deallocate excess memory, the number
 * of bytes removed or -1.
 */

ssize_t strbuf_compact(strbuf_t *self) {
  size_t len = strbuf_length(self);
  size_t rem = self->len - len;
  char *buf = calloc(len + 1, 1);
  if (!buf) return -1;
  if (memcpy(buf, self->data, len)) {
    free(self->alloc);
    self->len = len;
    self->data = self->alloc = buf;
    return rem;
  }
  return -1;
}

/*
 * Free the buffer.
 */

void strbuf_free(strbuf_t *self) {
  if (!self) return;
  free(self->alloc);
  free(self);
}

/*
 * Return buffer size.
 */

size_t strbuf_size(strbuf_t *self) {
  if (!self) return 0;
  return self->len;
}

/*
 * Return string length.
 */

size_t strbuf_length(strbuf_t *self) {
  if (!self) return 0;
  return strlen(self->data);
}

/*
 * Resize to hold `n` bytes.
 */

int strbuf_resize(strbuf_t *self, size_t n) {
  size_t len = nearest_multiple_of(1024, n);
  char *tmp;
  if (!self) return -1;
  tmp = realloc(self->alloc, len + 1);
  if (!tmp) return -1;
  self->len = len;
  self->alloc = self->data = tmp;
  self->alloc[len] = '\0';
  return 0;
}

/*
 * Append a printf-style formatted string to the buffer.
 */

int strbuf_appendf(strbuf_t *self, const char *format, ...) {
  va_list ap;
  va_list tmpa;
  char *dst = NULL;
  int length = 0;
  int required = 0;
  int bytes = 0;

  va_start(ap, format);

  length = strbuf_length(self);

  // First, we compute how many bytes are needed
  // for the formatted string and allocate that
  // much more space in the buffer.
  va_copy(tmpa, ap);
  required = vsnprintf(NULL, 0, format, tmpa);
  va_end(tmpa);
  if (-1 == strbuf_resize(self, length + required)) {
    va_end(ap);
    return -1;
  }

  // Next format the string into the space that we
  // have made room for.
  dst = self->data + length;
  bytes = vsnprintf(dst, 1 + required, format, ap);
  va_end(ap);

  return bytes < 0 ? -1 : 0;
}

/*
 * Append `str` to `self` and return 0 on success, -1 on failure.
 */

int strbuf_append(strbuf_t *self, const char *str) {
  return strbuf_append_n(self, str, strlen(str));
}

/*
 * Append the first `len` bytes from `str` to `self` and
 * return 0 on success, -1 on failure.
 */
int strbuf_append_n(strbuf_t *self, const char *str, size_t len) {
  size_t prev, needed;
  if (!self || !str || len == 0) return -1;

  prev = strlen(self->data);
  needed = len + prev;

  // enough space
  if (self->len > needed) {
    return strncat(self->data, str, len) ? 0 : -1;
  }

  // resize
  if (strbuf_resize(self, needed) == -1) return -1;
  return strncat(self->data, str, len) ? 0 : -1;
}

/*
 * Prepend `str` to `self` and return 0 on success, -1 on failure.
 */

int strbuf_prepend(strbuf_t *self, const char *str) {
  size_t len, prev, needed;
  if (!self || !str || (len = strlen(str)) == 0) return -1;

  prev = strlen(self->data);
  needed = len + prev;

  // enough space
  if (self->len > needed) goto move;

  // resize
  if (strbuf_resize(self, needed) == -1) return -1;

  // move
move:
  if (!memmove(self->data + len, self->data, prev + 1)) return -1;
  if (!memcpy(self->data, str, len)) return -1;

  return 0;
}

/*
 * Return a new buffer based on the `from..to` slice of `buf`,
 * or NULL on error.
 */

strbuf_t * strbuf_slice(strbuf_t *buf, size_t from, ssize_t to) {
  size_t len, n;
  strbuf_t *self;
  if (!buf) return NULL;
  len = strlen(buf->data);
  if (to < 0) to = (ssize_t)len - ~to;

  // bad range
  if (to < (ssize_t)from) return NULL;

  // cap end
  if (to > (ssize_t)len) to = (ssize_t)len;

  n = (size_t)to - from;
  self = strbuf_new_with_size(n);
  if (memcpy(self->data, buf->data + from, n)) {
    return self;
  }
  strbuf_free(self);
  return NULL;
}

/*
 * Return 1 if the buffers contain equivalent data.
 */

int strbuf_equals(strbuf_t *self, strbuf_t *other) {
  if (!self && !other) return 1;
  if (!self || !other) return 0;
  return 0 == strcmp(self->data, other->data);
}

/*
 * Return the index of the substring `str`, or -1 on failure.
 */

ssize_t strbuf_indexof(strbuf_t *self, const char *str) {
  char *sub = NULL;
  if (!self || !str || strlen(str) == 0) return -1;
  sub = strstr(self->data, str);
  if (!sub) return -1;
  return (sub - self->data);
}

static int is_delim(int ch, const char * chars) {
  if (!chars || strlen(chars) == 0) return isspace(ch);
  return strchr(chars, ch) != NULL;
}

/*
 * Trim leading whitespace.
 */

void strbuf_ltrim(strbuf_t *self, const char *chars) {
  int c;
  if (!self) return;
  while ((c = *self->data) && is_delim(c, chars)) {
    ++self->data;
  }
}

/*
 * Trim trailing whitespace.
 */

void strbuf_rtrim(strbuf_t *self, const char *chars) {
  int c;
  if (!self) return;
  if (strbuf_length(self) == 0) return;
  size_t i = strbuf_length(self) - 1;
  while ((c = self->data[i]) && is_delim(c, chars)) {
    self->data[i--] = 0;
  }
}

/*
 * Trim trailing and leading whitespace.
 */

void strbuf_trim(strbuf_t *self, const char *chars) {
  if (!self) return;
  strbuf_ltrim(self, chars);
  strbuf_rtrim(self, chars);
}

/*
 * Fill the buffer with `c`.
 */

void strbuf_fill(strbuf_t *self, int c) {
  if (!self) return;
  memset(self->data, c, self->len);
}

/*
 * Fill the buffer with 0.
 */

void strbuf_clear(strbuf_t *self) {
  if (!self) return;
  strbuf_fill(self, 0);
}

/*
 * Print a hex dump of the buffer.
 */

void strbuf_print(strbuf_t *self) {
  size_t i, len;
  if (!self) return;

  len = self->len;

  printf("\n ");

  // hex
  for (i = 0; i < len; ++i) {
    printf(" %02x", self->alloc[i]);
    if ((i + 1) % 8 == 0) printf("\n ");
  }

  printf("\n");
}
