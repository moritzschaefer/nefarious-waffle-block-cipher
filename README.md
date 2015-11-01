# nefarious-waffle-block-cipher

Paper and Implementation of our block cipher. Don't use in production ;)

# Specs

- Round function: Feistel-Structure
- Round number: 20
- Block size: 128 Bits
- Key size: 256 Bits
- S-Box: exp-based non-linear function

Only works for input data which has size of a multiple of the block size.

# Usage

Just use cipher.h/cipher.c

Make sure to run generate_sbox before using the cript function. You use cript for both encription and decription (note the boolean 'encrypt').
