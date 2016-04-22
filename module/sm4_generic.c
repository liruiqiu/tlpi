/*
 * Cryptographic API.
 *
 * SM4 Cipher Algorithm.
 *
 * Based on Brian Gladman's code.
 *
 * Linux developers:
 *  Alexander Kjeldaas <astor@fast.no>
 *  Herbert Valerio Riedel <hvr@hvrlab.org>
 *  Kyle McMartin <kyle@debian.org>
 *  Adam J. Richter <adam@yggdrasil.com> (conversion to 2.5 API).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 * All rights reserved.
 *
 * LICENSE TERMS
 *
 * The free distribution and use of this software in both source and binary
 * form is allowed (with or without changes) provided that:
 *
 *   1. distributions of this source code include the above copyright
 *      notice, this list of conditions and the following disclaimer;
 *
 *   2. distributions in binary form include the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other associated materials;
 *
 *   3. the copyright holder's name is not used to endorse products
 *      built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this product
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
 * ---------------------------------------------------------------------------
 */

#include <crypto/sm4.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/crypto.h>
#include <asm/byteorder.h>

#define ROUND 32

//static void test2(struct crypto_tfm *tfm, struct crypto_sm4_ctx *ctx);

const unsigned int crypto_sm4_tab[256] =
{0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05,
    0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99,
    0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62,
    0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6,
    0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8,
    0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35,
    0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87,
    0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e,
    0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1,
    0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3,
    0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f,
    0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51,
    0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8,
    0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0,
    0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84,
    0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48};

unsigned int crypto_sm4_ck_tab[32] = {
    0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
    0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
    0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
    0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
    0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
    0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
    0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
    0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};

EXPORT_SYMBOL_GPL(crypto_sm4_tab);


void byte_to_int(unsigned int *out, const unsigned char *in)
{
    int i,j;
    for(j=0;j<4;j++)
    {
        i=j<<2;
        out[j]=((u32)in[i]<<24)|((u32)in[i+1]<<16)|((u32)in[i+2]<<8)|((u32)in[i+3]);
    }   
}

void int_to_byte(unsigned char *out,const unsigned int *in )
{
    int j,i,s;
    for (j=0;j<4;j++)
    {
        i=j<<2;s=4-1-j;
        out[i]=in[s]>>24;
        out[i+1]=in[s]>>16;
        out[i+2]=in[s]>>8;
        out[i+3]=in[s];
    }
}
/*
*/
static inline u32 rotate_left(u32 x, u8 n)
{
    return ((x << n) | (x >> (32 - n)));
}

static inline u32 L_trans(u32 x)
{
    return (x ^ rotate_left(x, 2) ^ rotate_left(x, 10)
            ^ rotate_left(x, 18) ^ rotate_left(x, 24));
}

static inline u32 key_L_trans(u32 x)
{
    return (x ^ rotate_left(x, 13) ^ rotate_left(x, 23));
}

static inline u32 s_substitute(u32 x)
{
    u8 *px = (u8 *)&x;
    px[0] = crypto_sm4_tab[px[0]];
    px[1] = crypto_sm4_tab[px[1]];
    px[2] = crypto_sm4_tab[px[2]];
    px[3] = crypto_sm4_tab[px[3]];

    return x;
}

static inline u32 T_trans(u32 x)
{
    return (L_trans(s_substitute(x)));
}

static inline u32 key_T_trans(u32 x)
{
    return (key_L_trans(s_substitute(x)));
}

static inline void sm4_round_func(u32 *input, u32 sub_key)
{
    u32 tmp = input[0];

    input[0] = input[1];
    input[1] = input[2];
    input[2] = input[3];
    input[3] = tmp ^ T_trans(input[0] ^ input[1] ^ input[2] ^ sub_key);
}

static inline void sm4_reverse(u32 *input)
{
    u32 tmp;
    tmp = input[0], input[0] = input[3], input[3] = tmp;
    tmp = input[1], input[1] = input[2], input[2] = tmp;
}

static void sm4_key_schedule(u32 const *key, u32 *round_key)
{
    int j;
    u32 buf[4];
    u32 *prk = (u32 *)round_key;

    memcpy(buf, key, 16);

    buf[0] ^= 0xa3b1bac6; buf[1] ^= 0x56aa3350;
    buf[2] ^= 0x677d9197; buf[3] ^= 0xb27022dc;


    for(j = 0; j < ROUND; ++j){
        prk[j] = buf[0] ^ key_T_trans(buf[1] ^ buf[2] ^ buf[3] ^ crypto_sm4_ck_tab[j]);
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = prk[j];
    }
}



/**
 * crypto_sm4_expand_key - Expands the SM4 key as described in FIPS-197
 * @ctx:	The location where the computed key will be stored.
 * @in_key:	The supplied key.
 * @key_len:	The length of the supplied key.
 *
 * Returns 0 on success. The function fails only if an invalid key size (or
 * pointer) is supplied.
 * The expanded key size is 240 bytes (max of 14 rounds with a unique 16 bytes
 * key schedule plus a 16 bytes key which is used before the first round).
 * The decryption key is prepared for the "Equivalent Inverse Cipher" as
 * described in FIPS-197. The first slot (16 bytes) of each key (enc or dec) is
 * for the initial combination, the second slot for the first round and so on.
 */
int crypto_sm4_expand_key(struct crypto_sm4_ctx *ctx, const u8 *in_key,
        unsigned int key_len)
{

    u32 key[16]= {0};

    if (key_len != SM4_KEYSIZE_128 && key_len != SM4_KEYSIZE_192 &&
			key_len != SM4_KEYSIZE_256)
		return -EINVAL;

    ctx->key_length = key_len;
    byte_to_int(key, in_key);
    sm4_key_schedule(key, ctx->key_round);

    return 0;
}
EXPORT_SYMBOL_GPL(crypto_sm4_expand_key);

/**
 * crypto_sm4_set_key - Set the SM4 key.
 * @tfm:	The %crypto_tfm that is used in the context.
 * @in_key:	The input key.
 * @key_len:	The size of the key.
 *
 * Returns 0 on success, on failure the %CRYPTO_TFM_RES_BAD_KEY_LEN flag in tfm
 * is set. The function uses crypto_sm4_expand_key() to expand the key.
 * &crypto_sm4_ctx _must_ be the private data embedded in @tfm which is
 * retrieved with crypto_tfm_ctx().
 */
int crypto_sm4_set_key(struct crypto_tfm *tfm, const u8 *in_key,
        unsigned int key_len)
{
    struct crypto_sm4_ctx *ctx;
    u32 *flags;
    int ret;

    ctx = crypto_tfm_ctx(tfm);
    flags = &tfm->crt_flags;
    
    ret = crypto_sm4_expand_key(ctx, in_key, key_len);
    if (!ret)
        return 0;

    *flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
    return -EINVAL;
}
EXPORT_SYMBOL_GPL(crypto_sm4_set_key);

static void sm4_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
    const struct crypto_sm4_ctx *ctx;
    u32 b0[4] = {0};
    int i;

    ctx = crypto_tfm_ctx(tfm);

    byte_to_int(b0, in);

    for(i = 0; i < ROUND; ++i)
        sm4_round_func(b0, ctx->key_round[i]);

    int_to_byte(out,b0);
}

static void sm4_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
    const struct crypto_sm4_ctx *ctx;
    u32 b0[4] = {0};
    int i;

    ctx = crypto_tfm_ctx(tfm);

    byte_to_int(b0, in);

    for(i = 0; i < ROUND; ++i)
        sm4_round_func(b0, ctx->key_round[ROUND-1-i]);

    int_to_byte(out,b0);
}

static struct crypto_alg sm4_alg = {
    .cra_name		=	"sm4",
    .cra_driver_name	=	"sm4-generic",
    .cra_priority		=	100,
    .cra_flags		=	CRYPTO_ALG_TYPE_CIPHER,
    .cra_blocksize		=	SM4_BLOCK_SIZE,
    .cra_ctxsize		=	sizeof(struct crypto_sm4_ctx),
    .cra_alignmask		=	3,
    .cra_module		=	THIS_MODULE,
    .cra_list		=	LIST_HEAD_INIT(sm4_alg.cra_list),
    .cra_u			=	{
        .cipher = {
            .cia_min_keysize	=	SM4_MIN_KEY_SIZE,
            .cia_max_keysize	=	SM4_MAX_KEY_SIZE,
            .cia_setkey		=	crypto_sm4_set_key,
            .cia_encrypt		=	sm4_encrypt,
            .cia_decrypt		=	sm4_decrypt
        }
    }
};

static int __init sm4_init(void)
{
    return crypto_register_alg(&sm4_alg);
}

static void __exit sm4_fini(void)
{
    crypto_unregister_alg(&sm4_alg);
}

module_init(sm4_init);
module_exit(sm4_fini);

MODULE_DESCRIPTION("Rijndael (SM4) Cipher Algorithm");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("sm4");
