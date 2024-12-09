#include <stdlib.h>
#include <string.h>
#include "uri.h"

#define is_hex(x) ( \
  (((x) >= '0' && (x) <= '9') || ((x) >= 'A' && (x) <= 'F') || \
  ((x) >= 'a' && (x) <= 'f')) ? 1 : 0 \
)
#define is_chr(x) ( \
  (((x) == '=') || ((x) == '#') || ((x) == '&') || ((x) == '\0')) ? 0 : 1 \
)
#define hex2dec(x) ( \
  ((x) >= '0' && (x) <= '9') ? (x) - 48 : ((x) >= 'A' && (x) <= 'F') ? \
  (x) - 55 : ((x) >= 'a' &&(x) <= 'f') ? (x) - 87 : 0 \
)
#define is_char(x) ( \
  (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z') || \
  ('0' <= (x) && (x) <= '9')) ? 1 : 0 \
)

char * urlencode(const char *qs, int plus) {
  char * dst = NULL;
  const char *hex = "0123456789abcdef";
  size_t len, i, j = 0;
  if (!qs || (len = strlen(qs)) == 0) return dst;
  dst = malloc(sizeof(char) * (len * 3 + 1));
  if (!dst) return NULL;
  for (i = 0; i < len; i++) {
    if (qs[i] == ' ') {
      if (plus) {
        dst[j++] = '+';
      } else {
        dst[j++] = '%';
        dst[j++] = hex[(qs[i] >> 4) & 0x0f];
        dst[j++] = hex[qs[i] & 15];
      }
    } else if (is_char(qs[i])) {
      dst[j++] = qs[i];
    } else {
      dst[j++] = '%';
      dst[j++] = hex[(qs[i] >> 4) & 0x0f];
      dst[j++] = hex[qs[i] & 15];
    }
  }
  dst[j] = '\0';
  return dst;
}
int urldecode(char * qs) {
  int i = 0, j = 0;
  while (is_chr(qs[j])) {
    if (qs[j] == '+') {
      qs[i] = ' ';
    } else if (qs[j] == '%') {
      if (!is_hex(qs[j + 1]) || !is_hex(qs[j + 2])) {
        qs[i] = '\0';
        return i;
      }
      qs[i] = (hex2dec(qs[j + 1]) * 16) + hex2dec(qs[j + 2]);
      j += 2;
    } else {
      qs[i] = qs[j];
    }
    i++, j++;
  }
  qs[i] = '\0';
  return i;
}
