#include "cipher.h"


double time_diff(struct timeval end, struct timeval start) {
    return (end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000000.0;
}

int main(int argc, char *argv[])
{
    struct timeval start_time, sbox_generation_time, encryption_time, decryption_time, srand_time;
    gettimeofday(&srand_time, NULL);
    srand(srand_time.tv_usec);

    // generate random 1 MByte text
    byte *input = malloc(1024 * 1024);
    byte *output = malloc(1024 * 1024);
    byte *reversed_input = malloc(1024 * 1024);

    byte *key = malloc(256);
    int i;
    for (i = 0; i < 1024*1024; ++i) {
        input[i] = rand()%256;

    }
    memcpy(output, input, 1024*1024);
    for (i = 0; i < 256; ++i) {
        key[i] = rand()%256;
    }

    gettimeofday(&start_time, NULL);
    generate_sbox();
    gettimeofday(&sbox_generation_time, NULL);

    crypt(output, key, 1024*1024, 1);
    gettimeofday(&encryption_time, NULL);

    memcpy(reversed_input, output, 1024*1024);
    crypt(reversed_input, key, 1024*1024, 0);
    gettimeofday(&decryption_time, NULL);

    // compareend.tv_sec-start.tv_sec
    printf("SBox: %lf, crypt: %lf, decrypt: %lf \n", time_diff(sbox_generation_time, start_time), time_diff(encryption_time,sbox_generation_time), time_diff(decryption_time, encryption_time));
    if(memcmp(reversed_input, input, 1024*1024) == 0)
        printf("input == D(E(input)). SUCCESS!\n");
    else
        printf("input != D(E(input)). FAIL!\n");

    return 0;
}
