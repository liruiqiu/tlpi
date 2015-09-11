/*
 * ============================================================================
 *
 *       Filename:  sms4.h
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
#ifndef SMS4_INCLUDED
#define SMS4_INCLUDED

void sms4_encrypt(void *plaintext, void const *key);
void sms4_decrypt(void *ciphertext, void const *key);

#endif /* SMS4_INCLUDED */
