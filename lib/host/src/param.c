#include "param.h"
pthread_mutex_t mutex;
unsigned long current_untrusted_ptr = DEFAULT_UNTRUSTED_PTR;

void enclave_param_init(struct enclave_args* enclave_args)
{
  enclave_args->type = NORMAL_ENCLAVE;
  enclave_args->stack_size = DEFAULT_STACK_SIZE;
  enclave_args->untrusted_mem_ptr = DEFAULT_UNTRUSTED_PTR;
  enclave_args->untrusted_mem_size=DEFAULT_UNTRUSTED_SIZE;
}

void enclave_param_destroy(struct enclave_args* enclave_args)
{

}

void* alloc_untrusted_mem(struct enclave_args* enclave_args,unsigned long size)
{
  size = (size + 4*1024) & (~(4*1024));
  
  if(pthread_mutex_lock(&mutex) != 0)
  {
    fprintf(stderr,"LIB: Param: set_untrusted_mem get lock is failed \n");
    return NULL;
  }
  unsigned long ptr = (unsigned long)mmap((char*)current_untrusted_ptr, DEFAULT_UNTRUSTED_SIZE + size,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
  if(ptr == (unsigned long)MAP_FAILED)
  {
    pthread_mutex_unlock(&mutex);
    fprintf(stderr,"LIB: Param: failed to map %ld bytes memory\n",size + DEFAULT_UNTRUSTED_SIZE);
    return NULL;
  }
  if(ptr != current_untrusted_ptr)
  {
    pthread_mutex_unlock(&mutex);
    fprintf(stderr, "LIB: Param: failed to map at 0x%lx\n",current_untrusted_ptr);
    return NULL;
  }
  //DEFAULT UNTRUSTED MEM IS RESERVED FOR SYSCALL
  enclave_args->untrusted_mem_ptr = current_untrusted_ptr + DEFAULT_UNTRUSTED_SIZE;
  enclave_args->untrusted_mem_size = size;
  current_untrusted_ptr += (size + DEFAULT_UNTRUSTED_SIZE);
  pthread_mutex_unlock(&mutex);
  memset((char *)ptr, 0 ,DEFAULT_UNTRUSTED_SIZE + size);
  return (char *)(enclave_args->untrusted_mem_ptr); 
}




