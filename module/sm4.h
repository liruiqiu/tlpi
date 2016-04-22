/*
 *  * Common values for SM4 algorithms
 *   
*/

#ifndef _CRYPTO_SM4_H
#define _CRYPTO_SM4_H

#include <linux/types.h>
#include <linux/crypto.h>

#define SM4_MIN_KEY_SIZE    16
#define SM4_MAX_KEY_SIZE    32
#define SM4_KEYSIZE_128     16
#define SM4_KEYSIZE_192     24
#define SM4_KEYSIZE_256     32
#define SM4_BLOCK_SIZE      16
#define SM4_MAX_KEYLENGTH   (15 * 16)
#define SM4_MAX_KEYLENGTH_U32   (SM4_MAX_KEYLENGTH / sizeof(u32))

/*
 *  * Please ensure that the first two fields are 16-byte aligned
 *   * relative to the start of the structure, i.e., don't move them!
 *    */
struct crypto_sm4_ctx {
  //  u32 key_enc[SM4_MAX_KEYLENGTH_U32];
  //  u32 key_dec[SM4_MAX_KEYLENGTH_U32];
    u32 key_round[32];
    u32 key_length;
};

extern const u32 crypto_sm4_tab[256];

int crypto_sm4_set_key(struct crypto_tfm *tfm, const u8 *in_key,
        unsigned int key_len);
int crypto_sm4_expand_key(struct crypto_sm4_ctx *ctx, const u8 *in_key,
        unsigned int key_len);
#endif
