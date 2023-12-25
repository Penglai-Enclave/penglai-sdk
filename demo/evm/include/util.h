#ifndef PENGLAI_EVM_UTIL_H
#define PENGLAI_EVM_UTIL_H

#include <cstdint>
#include <cstring>
#include "SM4_Enc.h"
#include "Random.h"
#include "./uint256/uint256_t.h"

void store_uint256_to_uint8_vector(const uint256_t& value, uint8_t* to);
uint256_t uint8_vector_to_uint256(const uint8_t* from, size_t size);
void print_uint256(const uint256_t& value, int base);
void printHex(unsigned char *c, int n);
void encrypt_data(unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len);
void decrypt_data(unsigned char* cipher_with_tag, int cipher_with_tag_len, unsigned char* plaintext, int* plaintext_len);

#endif