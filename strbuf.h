
//
// buffer.h
//
// Copyright (c) 2012 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef STRBUF_H
#define STRBUF_H 1

#include <sys/types.h>

/*
 * Default buffer size.
 */

#ifndef STRBUF_DEFAULT_SIZE
#define STRBUF_DEFAULT_SIZE 64
#endif

/*
 * Buffer struct.
 */

typedef struct {
  size_t len;
  char *alloc;
  char *data;
} strbuf_t;

// prototypes

strbuf_t * strbuf_new();

strbuf_t * strbuf_new_with_size(size_t n);

strbuf_t * strbuf_new_with_string(const char *str);

strbuf_t * strbuf_new_with_string_length(const char *str, size_t len);

strbuf_t * strbuf_new_with_copy(const char *str);

size_t strbuf_size(strbuf_t *self);

size_t strbuf_length(strbuf_t *self);

void strbuf_free(strbuf_t *self);

int strbuf_prepend(strbuf_t *self, const char *str);

int strbuf_append(strbuf_t *self, const char *str);

int strbuf_appendf(strbuf_t *self, const char *format, ...);

int strbuf_append_n(strbuf_t *self, const char *str, size_t len);

int strbuf_equals(strbuf_t *self, strbuf_t *other);

ssize_t strbuf_indexof(strbuf_t *self, const char *str);

strbuf_t * strbuf_slice(strbuf_t *self, size_t from, ssize_t to);

ssize_t strbuf_compact(strbuf_t *self);

void strbuf_fill(strbuf_t *self, int c);

void strbuf_clear(strbuf_t *self);

void strbuf_ltrim(strbuf_t *self, const char *chars);

void strbuf_rtrim(strbuf_t *self, const char *chars);

void strbuf_trim(strbuf_t *self, const char *chars);

void strbuf_print(strbuf_t *self);

#define strbuf_string(self) (self->data)

#endif
