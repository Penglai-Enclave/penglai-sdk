#include "ocall.h"
#include "eapp.h"
void* eapp_mmap(void* vaddr, unsigned long size)
{
  void* retval = 0;
  retval = (void*)EAPP_MMAP(OCALL_MMAP, vaddr, size);
  return retval;
}

int eapp_unmap(void* vaddr, unsigned long size)
{
  int retval = 0;
  retval = EAPP_UNMAP(OCALL_UNMAP, vaddr, size);
  return retval;
}