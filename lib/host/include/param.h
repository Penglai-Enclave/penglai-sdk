#ifndef _ENCLAVE_PARAM
#define _ENCLAVE_PARAM
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <pthread.h>
#include <string.h>

#define PENGLAI_ENCLAVE_IOC_MAGIC  0xa4

#define PENGLAI_ENCLAVE_IOC_CREATE_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x00, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_RUN_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x01, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_ATTEST_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x02, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_STOP_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x03, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_RESUME_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x04, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_DESTROY_ENCLAVE \
  _IOW(PENGLAI_ENCLAVE_IOC_MAGIC, 0x05, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_DEBUG_PRINT \
  _IOW(PENGLAI_ENCLAVE_IOC_MAGIC, 0x06, struct penglai_enclave_user_param)


#define DEFAULT_STACK_SIZE      1024*1024 // 1 MB
#define DEFAULT_UNTRUSTED_PTR   0x0000001000000000
#define DEFAULT_UNTRUSTED_SIZE  8192 // 8 KB

struct penglai_enclave_user_param
{
  unsigned long eid;
  unsigned long elf_ptr;
  long elf_size;
  long stack_size;
  unsigned long untrusted_mem_ptr;
  long untrusted_mem_size;
};

struct enclave_args
{
  unsigned long stack_size;
  unsigned long untrusted_mem_ptr;
  unsigned long untrusted_mem_size;
};

void enclave_param_init(struct enclave_args* enclave_args);
void enclave_param_destroy(struct enclave_args* enclave_args);
void* alloc_untrusted_mem(struct enclave_args* enclave_args,unsigned long size);

typedef unsigned char byte;
#define MD_SIZE 64
#define SIGNATURE_SIZE 64
#define PRIVATE_KEY_SIZE 64
#define PUBLIC_KEY_SIZE 32


#endif
