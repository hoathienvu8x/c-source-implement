#ifndef _HMAC_H
#define _HMAC_H

#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA384_DIGEST_SIZE ( 384 / 8)
#define SHA512_DIGEST_SIZE ( 512 / 8)

#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)
#define SHA384_BLOCK_SIZE  SHA512_BLOCK_SIZE

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned long long uint64;

typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA256_BLOCK_SIZE];
    uint32 h[8];
} sha256_ctx;

typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA512_BLOCK_SIZE];
    uint64 h[8];
} sha512_ctx;

typedef sha512_ctx sha384_ctx;

typedef struct {
    sha256_ctx ctx_inside;
    sha256_ctx ctx_outside;

    /* for hmac_reinit */
    sha256_ctx ctx_inside_reinit;
    sha256_ctx ctx_outside_reinit;

    unsigned char block_ipad[SHA256_BLOCK_SIZE];
    unsigned char block_opad[SHA256_BLOCK_SIZE];
} hmac_sha256_ctx;

typedef struct {
    sha384_ctx ctx_inside;
    sha384_ctx ctx_outside;

    /* for hmac_reinit */
    sha384_ctx ctx_inside_reinit;
    sha384_ctx ctx_outside_reinit;

    unsigned char block_ipad[SHA384_BLOCK_SIZE];
    unsigned char block_opad[SHA384_BLOCK_SIZE];
} hmac_sha384_ctx;

typedef struct {
    sha512_ctx ctx_inside;
    sha512_ctx ctx_outside;

    /* for hmac_reinit */
    sha512_ctx ctx_inside_reinit;
    sha512_ctx ctx_outside_reinit;

    unsigned char block_ipad[SHA512_BLOCK_SIZE];
    unsigned char block_opad[SHA512_BLOCK_SIZE];
} hmac_sha512_ctx;

void sha256_init(sha256_ctx * ctx);
void sha256_update(sha256_ctx *ctx, const unsigned char *message,
                   unsigned int len);
void sha256_final(sha256_ctx *ctx, unsigned char *digest);
void sha256(const unsigned char *message, unsigned int len,
            unsigned char *digest);

void sha384_init(sha384_ctx *ctx);
void sha384_update(sha384_ctx *ctx, const unsigned char *message,
                   unsigned int len);
void sha384_final(sha384_ctx *ctx, unsigned char *digest);
void sha384(const unsigned char *message, unsigned int len,
            unsigned char *digest);

void sha512_init(sha512_ctx *ctx);
void sha512_update(sha512_ctx *ctx, const unsigned char *message,
                   unsigned int len);
void sha512_final(sha512_ctx *ctx, unsigned char *digest);
void sha512(const unsigned char *message, unsigned int len,
            unsigned char *digest);

void hmac_sha256_init(hmac_sha256_ctx *ctx, const unsigned char *key,
                      unsigned int key_size);
void hmac_sha256_reinit(hmac_sha256_ctx *ctx);
void hmac_sha256_update(hmac_sha256_ctx *ctx, const unsigned char *message,
                        unsigned int message_len);
void hmac_sha256_final(hmac_sha256_ctx *ctx, unsigned char *mac,
                       unsigned int mac_size);
void hmac_sha256(const unsigned char *key, unsigned int key_size,
                 const unsigned char *message, unsigned int message_len,
                 unsigned char *mac, unsigned mac_size);

void hmac_sha384_init(hmac_sha384_ctx *ctx, const unsigned char *key,
                      unsigned int key_size);
void hmac_sha384_reinit(hmac_sha384_ctx *ctx);
void hmac_sha384_update(hmac_sha384_ctx *ctx, const unsigned char *message,
                        unsigned int message_len);
void hmac_sha384_final(hmac_sha384_ctx *ctx, unsigned char *mac,
                       unsigned int mac_size);
void hmac_sha384(const unsigned char *key, unsigned int key_size,
                 const unsigned char *message, unsigned int message_len,
                 unsigned char *mac, unsigned mac_size);

void hmac_sha512_init(hmac_sha512_ctx *ctx, const unsigned char *key,
                      unsigned int key_size);
void hmac_sha512_reinit(hmac_sha512_ctx *ctx);
void hmac_sha512_update(hmac_sha512_ctx *ctx, const unsigned char *message,
                        unsigned int message_len);
void hmac_sha512_final(hmac_sha512_ctx *ctx, unsigned char *mac,
                       unsigned int mac_size);
void hmac_sha512(const unsigned char *key, unsigned int key_size,
                 const unsigned char *message, unsigned int message_len,
                 unsigned char *mac, unsigned mac_size);

#endif /* _HMAC_H */
