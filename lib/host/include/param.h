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
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x02, struct penglai_enclave_attest_param)
#define PENGLAI_ENCLAVE_IOC_STOP_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x03, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_RESUME_ENCLAVE \
  _IOR(PENGLAI_ENCLAVE_IOC_MAGIC, 0x04, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_DESTROY_ENCLAVE \
  _IOW(PENGLAI_ENCLAVE_IOC_MAGIC, 0x05, struct penglai_enclave_user_param)
#define PENGLAI_ENCLAVE_IOC_DEBUG_PRINT \
  _IOW(PENGLAI_ENCLAVE_IOC_MAGIC, 0x06, struct penglai_enclave_user_param)

#define PRIVILEGE_U_MODE        0
#define PRIVILEGE_S_MODE        1

#define DEFAULT_STACK_SIZE      1024*1024 // 1 MB
#define DEFAULT_UNTRUSTED_PTR   0x0000001000000000
#define DEFAULT_UNTRUSTED_SIZE  8192 // 8 KB
#define DEFAULT_PRIVILEGE       PRIVILEGE_U_MODE

#define USER_PARAM_RESUME_FROM_CUSTOM_OCALL  1000

#define PRIVATE_KEY_SIZE       32
#define PUBLIC_KEY_SIZE        64
#define HASH_SIZE              32
#define SIGNATURE_SIZE         64

// Atestation-related structure
struct sm_report_t
{
  unsigned char hash[HASH_SIZE];
  unsigned char signature[SIGNATURE_SIZE];
  unsigned char sm_pub_key[PUBLIC_KEY_SIZE];
};

struct enclave_report_t
{
  unsigned char hash[HASH_SIZE];
  unsigned char signature[SIGNATURE_SIZE];
  uintptr_t nonce;
};

struct report_t
{
  struct sm_report_t sm;
  struct enclave_report_t enclave;
  unsigned char dev_pub_key[PUBLIC_KEY_SIZE];
};

struct signature_t
{
  unsigned char r[PUBLIC_KEY_SIZE/2];
  unsigned char s[PUBLIC_KEY_SIZE/2];
};

struct penglai_enclave_user_param
{
  unsigned long eid;
  unsigned long elf_ptr;
  long elf_size;
  long stack_size;
  unsigned long untrusted_mem_ptr;
  long untrusted_mem_size;
  long ocall_buf_size;
  int resume_type;
  int privilege;
};

struct penglai_enclave_attest_param
{
  unsigned long eid;
	unsigned long nonce;
	struct report_t report;
};

struct enclave_args
{
  unsigned long stack_size;
  unsigned long untrusted_mem_ptr;
  unsigned long untrusted_mem_size;
  int privilege;
};

void enclave_param_init(struct enclave_args* enclave_args);
void enclave_param_destroy(struct enclave_args* enclave_args);
void* alloc_untrusted_mem(struct enclave_args* enclave_args,unsigned long size);

typedef unsigned char byte;

#endif
