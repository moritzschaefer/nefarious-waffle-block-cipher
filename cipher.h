#ifndef CIPHER_H
#define CIPHER_H value
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


// const unsigned int KEY_SIZE_BYTES = 32;
// const unsigned int BLOCK_SIZE_BYTE = 16;
// const unsigned int HALF_BLOCK_SIZE_BYTE = 8;
// const unsigned int ROUNDS = 20;

typedef unsigned char byte;
typedef unsigned char bool;

void generate_sbox();
void crypt(byte *data, const byte *const key, unsigned int length, bool encrypt);

#endif
