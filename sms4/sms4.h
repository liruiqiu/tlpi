/*
 * ============================================================================
 *
 *       Filename:  sm4.h
 *
 *    Description:  Public interface for the SMS4 encryption algorithm.
 *
 *        Version:  1.0
 *        Created:  2012年04月01日 15时18分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Long, longcpp9@gmail.com
 *        Company:  SDU
 *
 * ============================================================================
*/
#ifndef SM4_INCLUDED
#define SM4_INCLUDED

void sm4_encrypt(void *plaintext, void const *key);
void sm4_decrypt(void *ciphertext, void const *key);

#endif /* SMS4_INCLUDED */
