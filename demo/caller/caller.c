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
  EAPP_RETURN(0);
}
