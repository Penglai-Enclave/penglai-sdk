#ifndef _PENGLAI_ENCLAVE
#define _PENGLAI_ENCLAVE
#include "penglai-enclave-page.h"
#include "penglai-enclave-elfloader.h"

#define ENCLAVE_IDR_MIN 0x1000
#define ENCLAVE_IDR_MAX 0xffff

#define EXTRA_PAGES 15
#define STACK_POINT 0x0000004000000000
#define PENGLAI_ENCLAVE_IOC_MAGIC  0xa4

//SBI CALL NUMBERS
#define SBI_SM_INIT                     100
#define SBI_SM_CREATE_ENCLAVE            99
#define SBI_SM_ATTEST_ENCLAVE            98
#define SBI_SM_RUN_ENCLAVE               97
#define SBI_SM_STOP_ENCLAVE              96
#define SBI_SM_RESUME_ENCLAVE            95
#define SBI_SM_DESTROY_ENCLAVE           94
#define SBI_SM_ALLOC_ENCLAVE_MEM         93
#define SBI_SM_MEMORY_EXTEND             92
#define SBI_SM_FREE_ENCLAVE_MEM          91
#define SBI_SM_DEBUG_PRINT               88
#define SBI_SM_CREATE_SERVER_ENCLAVE     84
#define SBI_SM_DESTROY_SERVER_ENCLAVE    83

//Error codes of SBI_SM_ALLOC_ENCLAVE_MEM
#define ENCLAVE_NO_MEMORY                -2
#define ENCLAVE_UNKNOWN_ERROR            -1
#define ENCLAVE_SUCCESS                   0
#define ENCLAVE_TIMER_IRQ                   1
#define ENCLAVE_OCALL                   2

/* OCALL codes */
#define OCALL_TIMER_IRQ                   2000
#define OCALL_MEMORY_EXTEND            2001
#define OCALL_MEMORY_FREE              2002
#define OCALL_SYSCALL                  2003
#define OCALL_MMAP                        1
#define OCALL_UNMAP                       2
#define OCALL_SYS_WRITE                   3

#define RESUME_FROM_SYSCALL               1
#define RESUME_FROM_TIMER_IRQ          2000
#define RESUME_FROM_MALLOC_PAGES       2001
#define RESUME_FROM_FREE_PAGES         2002
#define RESUME_FROM_STOP               2003
#define RESUME_FROM_OCALL              2004

#define FLAG_DESTROY                      0
#define DIRECT_DESTROY                    1
#define FREE_MAX_MEMORY                   2
#define FREE_SPEC_MEMORY                  3


#define PRE_EXTEND_MONITOR_MEMORY 1

/*Abstract for enclave */
#define ENCLAVE_DEFAULT_KBUFFER_ORDER           0
#define ENCLAVE_DEFAULT_KBUFFER_SIZE            ((1<<ENCLAVE_DEFAULT_KBUFFER_ORDER)*RISCV_PGSIZE)
#define NAME_LEN                                16

#ifndef _PENGLAI_ENCLAVE_TYPE
#define _PENGLAI_ENCLAVE_TYPE
typedef enum
{
  NORMAL_ENCLAVE = 0,
  SERVER_ENCLAVE = 1,
} enclave_type_t;
#endif

typedef struct penglai_enclave
{
  /* Allocated by secure monitor */
  unsigned int eid;
  char name[NAME_LEN];
  enclave_type_t type;

  untrusted_mem_t* untrusted_mem;
  enclave_mem_t* enclave_mem;
  vaddr_t kbuffer;
  unsigned long kbuffer_size;
  unsigned long ocall_func_id;
  unsigned long ocall_arg0;
  unsigned long ocall_arg1;
  unsigned long ocall_syscall_num;
} enclave_t;

typedef struct require_sec_memory
{
  unsigned long size;
  unsigned long paddr;
  unsigned long resp_size;
} require_sec_memory_t;

enclave_t* create_enclave(int total_pages, char* name, enclave_type_t type);
int destroy_enclave(enclave_t* enclave);
unsigned int enclave_idr_alloc(enclave_t* enclave);
enclave_t* enclave_idr_remove(unsigned int ueid); 
enclave_t* get_enclave_by_id(unsigned int ueid);

#endif
