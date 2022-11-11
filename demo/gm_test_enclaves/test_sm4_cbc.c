/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserved.

  This file test GM/T SM4_CBC and random generator implementation, check
  whether they work correctly in multiple times. CBC mode of operation for
  cryptographic block ciphers has simpler interfaces and best performance.
**************************************************************************/

#include "SM4_Enc.h"
#include "Random.h"

#include "eapp.h"
#include "print.h"
#include <string.h>

#define TEST_TIMES   10

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
	unsigned char iv[KEY_SIZE];
	unsigned char key[KEY_SIZE];
    penglai_read_rand(iv, KEY_SIZE);
    penglai_read_rand(key, KEY_SIZE);
    
    eapp_print("random initialized IV :\n");
    printHex(iv, KEY_SIZE);
    eapp_print("random initialized Key :\n");
    printHex(key, KEY_SIZE);
    
    unsigned char plaintext[256];
    unsigned char ciphertext[256];
    unsigned char decrypttext[256];

    for(int t = 0; t < TEST_TIMES; t++){
        // random plaintext length
        unsigned char randomLen;
        penglai_read_rand(&randomLen, 1);
        eapp_print("\n\nTime %d, plaintext len: %d\n", t, randomLen);

        int plaintextLen = (int)randomLen;
        int ciphertextLen = CBC_CipherText_Len(plaintextLen);
        int len;

        penglai_read_rand(plaintext, plaintextLen);

        eapp_print("plaintext, len = %d\n", plaintextLen);
        printHex(plaintext, plaintextLen);

        len = SM4_CBC_Encrypt(key, iv, plaintext, plaintextLen, ciphertext, ciphertextLen);
        eapp_print("Eecrypt----------\n");
        eapp_print("ciphertext, len = %d\n", len);
        printHex(ciphertext, len);

        len = SM4_CBC_Decrypt(key, iv, ciphertext, ciphertextLen, decrypttext, plaintextLen);
        eapp_print("Decrypt----------\n");
        eapp_print("decrypttext, len = %d\n", len);
        printHex(decrypttext, len);

        for(int i = 0; i < plaintextLen; i++){
            if(plaintext[i] != decrypttext[i]){
                eapp_print("[ERROR] Time %d, index %d diff\n", t, i);
            }
        }
    }

	return 0;
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  unsigned long ret = test(args);
  EAPP_RETURN((unsigned long)ret);
}
