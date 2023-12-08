#ifndef EVM_SHARED_PARAM_H
#define EVM_SHARED_PARAM_H

#include <stdint.h>
#include <stdlib.h>

/**** Trusted function marshalling structs. ****/
typedef struct _evm_shared_param_t
{
    uint8_t code[1000];
    uint64_t code_size;
    uint8_t param[200];
    uint8_t param_size;
    uint64_t gas;
    uint8_t value[32];

    uint8_t output[200];
    uint8_t output_size;
    uint8_t log[1000];
    uint64_t log_size;
    uint64_t last_gas;
} evm_shared_param_t;

#endif
