/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserved.

  This file test GM/T SM2 implementation with random generator, check whether:
  (1) Sign/Verify works well during multiple prikey generations and multiple random k generation.
  (2) Random generator's random seed is set correctly.

  Change INIT_TIMES and SIGN_TIMES to change test conditions.
**************************************************************************/

#include "SM2_sv.h"

#include "eapp.h"
#include "print.h"
#include <string.h>

#define HASH_SIZE 32
#define INIT_TIMES 10
#define SIGN_TIMES 10

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
    int temp;
	unsigned char dA[32]; // the private key
	unsigned char xA[32], yA[32]; // the public key
	unsigned char r[32], s[32]; // Signature

	unsigned char *message = (unsigned char *)"message digest"; //the message to be signed
	int len = strlen((const char *)message); //the length of message

    // begin test random generator
	for(int i = 0; i < INIT_TIMES; i++){
		temp = SM2_KeyGeneration(dA, xA, yA);
		if (temp){
			eapp_print("[ERROR] init_time %d: SM2_KeyGeneration failed\n", i);
			return temp;
		}
        eapp_print("[TRACE] init_time %d: random prikey:\n", i);
        printHex(dA, HASH_SIZE);

		for(int j = 0; j < SIGN_TIMES; j++){
			temp = SM2_Sign(message, len, dA, r, s);
			if (temp){
				eapp_print("[ERROR] init_time %d, sign_time %d: SM2_Sign failed\n", i, j);
				return temp;
			}
            // eapp_print("[TRACE] init_time %d, sign_time %d: signature r:\n", i, j);
            // printHex(r, HASH_SIZE);
			temp = SM2_Verify(message, len, xA, yA, r, s);
			if (temp){
				eapp_print("[ERROR] init_time %d, sign_time %d: SM2_Verify failed\n", i, j);
				return temp;
			}
            eapp_print("[TRACE] init_time %d, sign_time %d: SUCCESS", i, j);
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
