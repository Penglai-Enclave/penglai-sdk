#include "eapp.h"
#include <string.h>
#include <stdlib.h>

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;

  int run_server1 = 1;
  char server_name[16];
  strcpy(server_name, "test-server1");
  unsigned long server_handle = acquire_enclave(server_name);
  if(server_handle == -1UL)
  {
    strcpy(server_name, "test-server");
    server_handle = acquire_enclave(server_name);
    run_server1 = 0;
  }
  if(server_handle == -1UL)
  {
    EAPP_RETURN(-1UL);
  }

  struct call_enclave_arg_t call_arg;
  call_enclave(server_handle, &call_arg);
  EAPP_RETURN(0);
}
