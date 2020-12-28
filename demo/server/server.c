#include "eapp.h"

int EAPP_ENTRY main(){
unsigned long* args;
  EAPP_RESERVE_REG;
  char server_name[16];
  strcpy(server_name, "test-server");
  unsigned long server_handle = acquire_enclave(server_name);
  struct call_enclave_arg_t call_arg;
  call_enclave(server_handle, &call_arg);
  call_arg.resp_val = 3;
  SERVER_RETURN(&call_arg);
}
