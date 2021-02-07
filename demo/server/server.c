#include "eapp.h"
#include "print.h"

int EAPP_ENTRY main(){
  unsigned long* args;
  EAPP_RESERVE_REG;
  unsigned long arg0 = args[10];
  void* vaddr = (void*)args[11];
  unsigned long size = args[12];
  eapp_print("server begin to run!\n");
  struct call_enclave_arg_t ret_arg;
  ret_arg.req_vaddr = vaddr;
  ret_arg.req_size = size;
  ret_arg.resp_vaddr = 0;
  ret_arg.resp_size = 0;
  ret_arg.resp_val = 2;

  SERVER_RETURN(&ret_arg);
}
