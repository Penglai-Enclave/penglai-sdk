#include "../include/util.h"

extern "C" void eapp_print(const char*s, ...);

const unsigned char key[16]={0x98,0xff,0xf6,0x7e,0x64,0xe4,0x6b,0xe5,0xee,0x2e,0x05,0xcc,0x9a,0xf6,0xd0,0x12};
const unsigned char IV[16] ={0x2d,0xfb,0x42,0x9a,0x48,0x69,0x7c,0x34,0x00,0x6d,0xa8,0x86,0x9a,0xf6,0xd0,0x12};

void printHex(unsigned char *c, int n)
{
    int i, rounds;
    rounds = n / 16;
    for (i = 0; i < rounds; i++) {
        eapp_print("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                   *(c+i*16), *(c+i*16+1), *(c+i*16+2), *(c+i*16+3),
                   *(c+i*16+4), *(c+i*16+5), *(c+i*16+6), *(c+i*16+7),
                   *(c+i*16+8), *(c+i*16+9), *(c+i*16+10), *(c+i*16+11),
                   *(c+i*16+12), *(c+i*16+13), *(c+i*16+14), *(c+i*16+15));
    }
}

uint256_t bswap256(const uint256_t& value){
    uint64_t uint64_list[4] = {value.upper().upper(), value.upper().lower(), value.lower().upper(), value.lower().lower()};
    for(int i = 0; i < 4; i++){
        uint64_list[i] = __builtin_bswap64(uint64_list[i]);
    }
    uint128_t new_upper(uint64_list[3], uint64_list[2]);
    uint128_t new_lower(uint64_list[1], uint64_list[0]);
    uint256_t new_value(new_upper, new_lower);
    return new_value;
}

void store_uint256_to_uint8_vector(const uint256_t& value, uint8_t* to){
    uint256_t new_value = bswap256(value);
    std::memcpy(to, &new_value, sizeof(new_value));
}

uint256_t uint8_vector_to_uint256(const uint8_t* from, size_t size){
    uint8_t uint8_list[32] = {};
    int offset = 32 - size;
    std::memcpy(uint8_list + offset, from, size);
    uint256_t old_value;
    std::memcpy(&old_value, uint8_list, sizeof(old_value));
    return bswap256(old_value);
}

void print_uint256(const uint256_t& value, int base){
    std::string value_string = value.str(base);
    char* real_value_string = (char*)value_string.data();
    eapp_print("value = %s\n", real_value_string);
}

void encrypt_data(unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len){
    *ciphertext_len = OCB_CipherText_Len(plaintext_len);
    unsigned char cipher_with_tag[*ciphertext_len + OCB_TAG_SIZE];

    SM4_OCB_Encrypt(key, IV, (unsigned int)16, plaintext, plaintext_len, NULL, 0, cipher_with_tag);

    *ciphertext_len = (*ciphertext_len) + OCB_TAG_SIZE;
    for(int i = 0; i < *ciphertext_len; i++){
        ciphertext[i] = cipher_with_tag[i];
    }
}

void decrypt_data(unsigned char* cipher_with_tag, int cipher_with_tag_len, unsigned char* plaintext, int* plaintext_len){
    *plaintext_len = cipher_with_tag_len - OCB_TAG_SIZE;
    int ret;
    ret = SM4_OCB_Decrypt(key, IV, (unsigned int)16, cipher_with_tag, *plaintext_len, NULL, 0, plaintext);
    if(ret != 0){
        eapp_print("Fail Assenticate\n");
    }
}