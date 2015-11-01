#include "cipher.h"
byte sbox[256];
const unsigned int KEY_SIZE_BYTES = 32;
const unsigned int BLOCK_SIZE_BYTE = 16;
const unsigned int HALF_BLOCK_SIZE_BYTE = 8;
const unsigned int ROUNDS = 20;

void xor(const byte *const in1, const byte *const in2, byte *out) {
    int i;
    for (i = 0; i < HALF_BLOCK_SIZE_BYTE; ++i) {
        out[i] = in1[i] ^ in2[i];
    }
}

void mirror_key(const byte *const in, byte *out) {
    int i;
    for (i = 0; i < HALF_BLOCK_SIZE_BYTE; ++i) {
        out[i] = in[HALF_BLOCK_SIZE_BYTE-1-1];
        byte tmp_byte = 0;
        int byte_i;
        for (byte_i = 0; byte_i < 8; ++byte_i) {
            tmp_byte |= ((in[HALF_BLOCK_SIZE_BYTE-i-1] >> byte_i) & 1) << (7-byte_i);
        }
        out[i] = tmp_byte;
    }
}

// rotate whole key 3 bits
void permutate(byte *block) {
    byte last_overflow = block[HALF_BLOCK_SIZE_BYTE-1] >> 5;
    byte overflow;
    int i;
    for (i = 0; i < HALF_BLOCK_SIZE_BYTE; ++i) {
        overflow = block[i] >> 5;
        block[i] = (block[i] << 3) + last_overflow;
        last_overflow = overflow;
    }
}
void apply_sbox(byte *block) {
    int i, x, y;
    for (i = 0; i < HALF_BLOCK_SIZE_BYTE; ++i) {
        x = block[i] >> 4;
        y = block[i] & 0x0F;

        block[i] = sbox[x*16+y];
    }
}


// TODO add const
void feistel(const byte *const input, const byte *const subkey, byte *output) {
    byte tmp[HALF_BLOCK_SIZE_BYTE];

    // apply key
    xor(input, subkey, output);

    // apply sbox
    apply_sbox(output);

    // apply mirrored key
    mirror_key(subkey, tmp);
    xor(output, tmp, output);

    // permutate
    permutate(output);
}



void round_function(byte *l, byte *r, const byte *const subkey) {
    // NOTE: l and r are not swapped!
    // inplace apply round function
    byte tmp[HALF_BLOCK_SIZE_BYTE];
    feistel(r, subkey, tmp);
    xor(tmp, l, l);
}

void generate_sbox() {
    int i, x, y;
    for (i = 0; i < 256; ++i) {
        x = i % 16;
        y = i / 16;

        double exponent = (((x+1)*3+(y+3)*17)/9);
        sbox[x*16+y] = ((unsigned int)round(exp(exponent)))%256;
    }

}

void generate_subkeys(const byte *const key, byte *subkeys) {
    int i;
    int bit_rotate = 101;
    byte tmp[KEY_SIZE_BYTES];
    memcpy(tmp, key, KEY_SIZE_BYTES);

    for (i = 0; i < ROUNDS/4; ++i) {
        int byte_i;
        for (byte_i = 0; byte_i < KEY_SIZE_BYTES; ++byte_i) {
            double pos = ((256 - bit_rotate - 8 * byte_i)%256)/8.0;
            tmp[byte_i] = key[(int)floor(pos)] >> 8-(bit_rotate % 8) + key[(int)ceil(pos)] << (bit_rotate %8);
        }
        int64_t *int_keys = (int64_t*)tmp;
        int_keys[0] += 1;
        int_keys[1] += 1;
        int_keys[2] += 1;
        int_keys[3] += 1;
        memcpy(subkeys+KEY_SIZE_BYTES*i, tmp, KEY_SIZE_BYTES);
    }
}

void crypt(byte *data, const byte *const key, unsigned int length, bool encrypt) {
    if(length % BLOCK_SIZE_BYTE != 0) {
        fprintf(stderr,  "warning: unexpected behaviour with input of 'uneven' blocksize!");
    }
    // generate subkeys

    byte subkeys[HALF_BLOCK_SIZE_BYTE*ROUNDS];
    generate_subkeys(key, subkeys);


    int block_i;
    for (block_i = 0; block_i < length / BLOCK_SIZE_BYTE; ++block_i) {
        int round_i;
        byte *l = data+round_i*BLOCK_SIZE_BYTE;
        byte *r = data+round_i*BLOCK_SIZE_BYTE+HALF_BLOCK_SIZE_BYTE;
        for (round_i = 0; round_i < ROUNDS; ++round_i) {
            if(encrypt)
                round_function(l, r, subkeys+(round_i*HALF_BLOCK_SIZE_BYTE));
            else
                round_function(l, r, subkeys+(HALF_BLOCK_SIZE_BYTE*(ROUNDS-round_i-1)));
            byte *tmp = l;
            l = r;
            r = tmp;
        }
        byte tmp[HALF_BLOCK_SIZE_BYTE];
        memcpy(tmp, l, HALF_BLOCK_SIZE_BYTE);
        memcpy(l, r, HALF_BLOCK_SIZE_BYTE);
        memcpy(r, tmp, HALF_BLOCK_SIZE_BYTE);
    }
}
