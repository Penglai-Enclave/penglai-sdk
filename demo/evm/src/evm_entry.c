#include "eapp.h"
#include "print.h"
#include "ocall.h"
#include "../include/evm_shared_param.h"

#include <string.h>
#include <stdlib.h>

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  eapp_print("%s is running\n", "evm");

  evm_shared_param_t evm_shared_param;

  if(sizeof(evm_shared_param_t) > DEFAULT_UNTRUSTED_SIZE){
      eapp_print("Size to copy is larger than untrusted mem size \n");
      EAPP_RETURN(0);
  }

  // get arguements from untrusted mem to safe memory
  memcpy(&evm_shared_param, (const void *)DEFAULT_UNTRUSTED_PTR, sizeof(evm_shared_param_t));

  uint8_t evm_code[1000];
  memset(evm_code, 0, sizeof(evm_code));
  memcpy(&evm_code, evm_shared_param.code, sizeof(evm_shared_param.code));
  uint8_t evm_param[200];
  memset(evm_param, 0, sizeof(evm_param));
  memcpy(&evm_param, evm_shared_param.param, sizeof(evm_shared_param.param));
  uint8_t evm_value[32];
  memset(evm_value, 0, sizeof(evm_value));
  memcpy(&evm_value, evm_shared_param.value, sizeof(evm_shared_param.value));

  uint8_t evm_output[200];
  memset(evm_output, 0, sizeof(evm_output));
  uint8_t evm_output_size;
  uint8_t evm_log[1000];
  memset(evm_log, 0, sizeof(evm_log));
  uint64_t evm_log_size;
  uint64_t evm_last_gas;
  evm_main(evm_code, evm_shared_param.code_size,
           evm_param, evm_shared_param.param_size,
           evm_output, &evm_output_size,
           evm_log, &evm_log_size,
           evm_value, evm_shared_param.gas,
           &evm_last_gas);

  // enclave handle arguements
  memset(evm_shared_param.output, 0, sizeof(evm_shared_param.output));
  memcpy(&evm_shared_param.output, evm_output, sizeof(evm_output));
  evm_shared_param.output_size = evm_output_size;
  memset(evm_shared_param.log, 0, sizeof(evm_shared_param.log));
  memcpy(&evm_shared_param.log, evm_log, sizeof(evm_log));
  evm_shared_param.log_size = evm_log_size;
  evm_shared_param.last_gas = evm_last_gas;

  // trans output
  memcpy((void *)DEFAULT_UNTRUSTED_PTR, &evm_shared_param, sizeof(evm_shared_param_t));

  EAPP_RETURN(0);
}