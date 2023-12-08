/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserved.

  This file test GM/T SM4_OCB implementation, check whether it can assure
  both message authentication and privacy.
**************************************************************************/

#include "SM4_Enc.h"
#include "Random.h"

#include "eapp.h"
#include "print.h"
#include <string.h>

void printHex(unsigned char *c, int n)
{
	int i, rounds;
    rounds = n / 16;
	for (i = 0; i < rounds; i++) {
		eapp_print("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                    c[i*16], c[i*16+1], c[i*16+2], c[i*16+3],
                    c[i*16+4], c[i*16+5], c[i*16+6], c[i*16+7],
                    c[i*16+8], c[i*16+9], c[i*16+10], c[i*16+11],
                    c[i*16+12], c[i*16+13], c[i*16+14], c[i*16+15]);
	}
}

int test(unsigned long * args)
{
    const unsigned char key[16]={0x98,0xff,0xf6,0x7e,0x64,0xe4,0x6b,0xe5,0xee,0x2e,0x05,0xcc,0x9a,0xf6,0xd0,0x12};
    const unsigned char IV[16] ={0x2d,0xfb,0x42,0x9a,0x48,0x69,0x7c,0x34,0x00,0x6d,0xa8,0x86,0x9a,0xf6,0xd0,0x12};

    // plaintext length can be any number, just use 42 to show.
    unsigned char plaintext[42]={
        0x29,0xb9,0x1b,0x4a,0x68,0xa9,0x9f,0x97,0xc4,0x1c,0x75,0x08,0xf1,0x7a,0x5c,0x7a,
        0x7a,0xfc,0x9e,0x1a,0xca,0x83,0xe1,0x29,0xb0,0x85,0xbd,0x63,0x7f,0xf6,0x7c,0x01,
        0x29,0xb9,0x1b,0x4a,0x68,0xa9,0x9f,0x97,0xc4,0x1c
    };

    const unsigned char add_data[3*16]={
        0xa0,0xca,0x58,0x61,0xc0,0x22,0x6c,0x5b,0x5a,0x65,0x14,0xc8,0x2b,0x77,0x81,0x5a,
        0x9e,0x0e,0xb3,0x59,0xd0,0xd4,0x6d,0x03,0x33,0xc3,0xf2,0xba,0xe1,0x4d,0xa0,0xc4,
        0x03,0x30,0xc0,0x02,0x16,0xb4,0xaa,0x64,0xb7,0xc1,0xed,0xb8,0x71,0xc3,0x28,0xf6
    };

    int plaintextLen = 42;
    int ciphertextLen = OCB_CipherText_Len(plaintextLen);
    unsigned char cipher_with_tag[ciphertextLen + OCB_TAG_SIZE];
    unsigned char decrypttext[plaintextLen];

    eapp_print("plaintext :\n");
    printHex(plaintext, plaintextLen);

    SM4_OCB_Encrypt(key, IV, (unsigned int)16, plaintext, plaintextLen, add_data, 3*16, cipher_with_tag);
    eapp_print("ciphertext with tag :\n");
    printHex(cipher_with_tag, ciphertextLen + OCB_TAG_SIZE);

    int ret;
    ret = SM4_OCB_Decrypt(key, IV, (unsigned int)16, cipher_with_tag, ciphertextLen, add_data, 3*16, decrypttext);
    if(ret == 0){
        eapp_print("Success Authenticate\n");
    } else {
        eapp_print("Fail Assenticate\n");
    }
    eapp_print("decrypttext :\n");
    printHex(decrypttext, plaintextLen);
    for(int i = 0; i < plaintextLen; i++){
        if(decrypttext[i] != plaintext[i]){
            eapp_print("Decrypt diff at %d\n", i);
        }
    }

    // check if Authentication can be assure.
    unsigned char demaged_ciphertext[ciphertextLen + OCB_TAG_SIZE];
    memcpy(demaged_ciphertext, cipher_with_tag, ciphertextLen + OCB_TAG_SIZE);
    demaged_ciphertext[5] = 'a';
    eapp_print("What if change ciphertext?\n");
    ret = SM4_OCB_Decrypt(key, IV, (unsigned int)16, demaged_ciphertext, ciphertextLen, add_data, 3*16, decrypttext);
    if(ret == 0){
        eapp_print("Success Authenticate\n");
    } else {
        eapp_print("Fail Assenticate\n");
    }

    unsigned char demaged_add_data[3*16];
    memcpy(demaged_add_data, add_data, 3*16);
    demaged_add_data[3] = 0x58;
    eapp_print("What if change add_data?\n");
    ret = SM4_OCB_Decrypt(key, IV, (unsigned int)16, cipher_with_tag, ciphertextLen, demaged_add_data, 3*16, decrypttext);
    if(ret == 0){
        eapp_print("Success Authenticate\n");
    } else {
        eapp_print("Fail Assenticate\n");
    }

    eapp_print("What if change ciphertext and add_data?\n");
    ret = SM4_OCB_Decrypt(key, IV, (unsigned int)16, demaged_ciphertext, ciphertextLen, demaged_add_data, 3*16, decrypttext);
    if(ret == 0){
        eapp_print("Success Authenticate\n");
    } else {
        eapp_print("Fail Assenticate\n");
    }

	return 0;
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  unsigned long ret = test(args);
  EAPP_RETURN((unsigned long)ret);
}
