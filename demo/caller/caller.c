#include "eapp.h"
#include "print.h"
#include <string.h>
#include <stdlib.h>

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;

  eapp_print("caller begin to run\n");

  char server_name[16];
  strcpy(server_name, "test-server");
  unsigned long server_handle = acquire_enclave(server_name);
  if(server_handle == -1UL)
  {
    EAPP_RETURN(-1UL);
  }

  struct call_enclave_arg_t call_arg;
  call_arg.req_arg = 0;
  call_arg.req_vaddr = 0;
  call_arg.req_size = 0;
  eapp_print("caller: before call_enclave!\n");
  call_enclave(server_handle, &call_arg);
  eapp_print("caller: get server retval:0x%lx\n", call_arg.resp_val);
  EAPP_RETURN(0);
}
