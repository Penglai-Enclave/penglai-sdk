#include "../include/evm_shared_param.h"
#include "../include/instruction.h"
#include "penglai-enclave.h"
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <string>

#define NONCE 12345

struct args
{
  void* in;
  int i;
};

void run_enclave_with_args(struct PLenclave *enclave)
{
  uint8_t* untrusted_mem_extent = NULL;
  evm_shared_param_t evm_shared_param;

  // Fibonacci
  uint8_t evm_code[] = {96, 128, 96, 64, 82, 96, 4, 54, 16, 96, 63, 87, 96, 0, 53, 124, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 144, 4, 99,
                        255, 255, 255, 255, 22, 128, 99, 198, 136, 143, 161, 20, 96, 68, 87, 91, 96, 0, 128, 253, 91, 52, 128, 21, 96, 79, 87, 96, 0, 128, 253, 91, 80, 96, 108, 96, 4, 128,
                        54, 3, 129, 1, 144, 128, 128, 53, 144, 96, 32, 1, 144, 146, 145, 144, 80, 80, 80, 96, 130, 86, 91, 96, 64, 81, 128, 130, 129, 82, 96, 32, 1, 145, 80, 80, 96, 64, 81,
                        128, 145, 3, 144, 243, 91, 96, 0, 128, 96, 0, 128, 96, 0, 96, 1, 147, 80, 96, 1, 146, 80, 96, 0, 145, 80, 91, 96, 1, 134, 3, 130, 16, 21, 96, 184, 87, 130, 132, 1,
                        144, 80, 130, 147, 80, 128, 146, 80, 129, 128, 96, 1, 1, 146, 80, 80, 96, 151, 86, 91, 127, 36, 171, 219, 88, 101, 223, 80, 121, 220, 197, 172, 89, 15, 246, 240, 29,
                        92, 22, 237, 188, 95, 171, 78, 25, 93, 159, 235, 209, 17, 69, 3, 218, 132, 96, 64, 81, 128, 130, 129, 82, 96, 32, 1, 145, 80, 80, 96, 64, 81, 128, 145, 3, 144, 161,
                        131, 148, 80, 80, 80, 80, 80, 145, 144, 80, 86, 0, 161, 101, 98, 122, 122, 114, 48, 88, 32, 215, 202, 221, 69, 161, 76, 223, 51, 99, 251, 101, 42, 109, 247, 186, 48,
                        192, 93, 208, 228, 4, 78, 90, 59, 18, 125, 133, 21, 125, 112, 146, 146, 0, 41};

  memset(evm_shared_param.code, 0, sizeof(evm_shared_param.code));
  memcpy(&evm_shared_param.code, evm_code, sizeof(evm_code));
  evm_shared_param.code_size = sizeof(evm_code)/sizeof(evm_code[0]);
  uint8_t evm_param[] = {198, 136, 143, 161, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7};
  memset(evm_shared_param.param, 0, sizeof(evm_shared_param.param));
  memcpy(&evm_shared_param.param, evm_param, sizeof(evm_param));
  evm_shared_param.param_size = sizeof(evm_param)/sizeof(evm_param[0]);
  uint8_t evm_value[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memset(evm_shared_param.value, 0, sizeof(evm_shared_param.value));
  memcpy(&evm_shared_param.value, evm_value, sizeof(evm_value));
  evm_shared_param.gas = 5000000;
  untrusted_mem_extent = (uint8_t*)malloc(DEFAULT_UNTRUSTED_SIZE);
  memcpy(untrusted_mem_extent, &evm_shared_param, sizeof(evm_shared_param));

  //trans args by untrusted mem
  enclave->user_param.untrusted_mem_ptr = (unsigned long)untrusted_mem_extent;
  enclave->user_param.untrusted_mem_size = DEFAULT_UNTRUSTED_SIZE;
  
  PLenclave_run(enclave);

  //handle outputs
  evm_shared_param_t *evm_shared_param_after_run = (evm_shared_param_t *)untrusted_mem_extent;
  uint8_t evm_output[200];
  memset(evm_output, 0, sizeof(evm_output));
  memcpy(&evm_output, evm_shared_param_after_run->output, sizeof(evm_shared_param_after_run->output));
  std::vector<uint8_t> output;
  for(int i = 0; i < evm_shared_param_after_run->output_size; i++){
      output.push_back(evm_output[i]);
  }
  printf("output: \n");
  for(int i = 0; i < output.size(); i++){
      printf("%d, ", output[i]);
  }
  printf("\n");

  uint8_t evm_log[1000];
  memset(evm_log, 0, sizeof(evm_log));
  memcpy(&evm_log, evm_shared_param_after_run->log, sizeof(evm_shared_param_after_run->log));
  std::vector<uint8_t> log;
  for(int i = 0; i < evm_shared_param_after_run->log_size; i++){
      log.push_back(evm_log[i]);
  }
  printf("log: \n");
  for(int i = 0; i < log.size(); i++){
      printf("%d, ", log[i]);
  }
  printf("\n");
}

void* create_enclave(void* args0)
{
  struct args *args = (struct args*)args0;
  void* in = args->in;
  int i = args->i;
  int ret = 0;

  struct PLenclave* enclave = static_cast<PLenclave*>(malloc(sizeof(struct PLenclave)));
  struct enclave_args* params = static_cast<enclave_args*>(malloc(sizeof(struct enclave_args)));
  PLenclave_init(enclave);
  enclave_param_init(params);

  struct elf_args * enclaveFile = (struct elf_args *)in;
  params->untrusted_mem_size = DEFAULT_UNTRUSTED_SIZE;
  params->untrusted_mem_ptr = 0;
//  enclaveFile->size = 4096 * 4096;
  if(PLenclave_create(enclave, enclaveFile, params) < 0 )
  {
    printf("host:%d: failed to create enclave\n", i);
  }
  else
  {
      PLenclave_attest(enclave, NONCE);
//    printf("host:%d: enclave run\n", i);
      run_enclave_with_args(enclave);
  }
  PLenclave_finalize(enclave);
  enclave_param_destroy(params);
  free(enclave);
  free(params);
}

int main(int argc, char** argv)
{
  if(argc <= 1)
  {
    printf("Please input the enclave ELF file name\n");
  }

  struct args* args = (struct args*)malloc(sizeof(struct args));

  struct elf_args* enclaveFile = static_cast<elf_args*>(malloc(sizeof(struct elf_args)));
  char * eappfile = argv[1];
  elf_args_init(enclaveFile, eappfile);
  
  if(!elf_valid(enclaveFile))
  {
    printf("error when initializing enclaveFile\n");
    goto out;
  }

    args->in = (void*)enclaveFile;
    args->i = 1;
    create_enclave((void*)args);

out:
  elf_args_destroy(enclaveFile);
  free(enclaveFile);
  free(args);

  return 0;
}
