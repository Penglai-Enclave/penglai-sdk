/*********************************************************
 * This demo contains an enclave, which accept two numbers(int) A and
 * B as input, and caculate int C = A + B and its name as output.
 * The struct _get_name_t showed below is to perform args marshal.
 */

#ifndef HELLOWORLD_ARGS_H
#define HELLOWORLD_ARGS_H

#include <stdint.h>
#include <stdlib.h>

/**** Trusted function marshalling structs. ****/
typedef struct _get_name_t
{
    int A;                  // [in] number A 
    int B;                  // [in] number B
    int C;                  // [out] number C = A + B
    char enclave_name[20];  // [out] 
} get_name_t;

#endif
