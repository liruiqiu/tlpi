/*
 * ============================================================================
 *
 *       Filename:  sms4_test.c
 *
 *    Description:  Test the implementation of sms4.
 *
 *        Version:  1.0
 *        Created:  2012年04月01日 15时20分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Long, longcpp9@gmail.com
 *        Company:  SDU
 *
 * ============================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sms4.h"

void print(uint32_t *buf, int size)
{
int i = 0; 
for(i = 0; i < size; ++i)
printf("%08" PRIx32 " ", buf[i]);
printf("\n");
}

int main(int argc, const char *argv[])
{
uint32_t buffer[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
uint32_t key[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};

printf("the plaintext:\t");
print(buffer, 4);

printf("the key:\t");
print(key, 4);

sms4_encrypt(buffer, key);

printf("the ciphertext:\t");
print(buffer, 4);

sms4_decrypt(buffer, key);

printf("the plaintext:\t");
print(buffer, 4);

exit(EXIT_SUCCESS);
}
