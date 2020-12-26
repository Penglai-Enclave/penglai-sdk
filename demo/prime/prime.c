#include "eapp.h"
int prime_loop(int num)
{
  unsigned long count;
  int i;
  for(i = 2; i < num; i++)
  {
    if (num % i ==0)
      count++;
  }
  return count;
}

int prime(unsigned long * args)
{
  unsigned long ret;
  ret = prime_loop(111);
  EAPP_RETURN(ret);
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  unsigned long server_handle = acquire_enclave("hhh");
  prime(args);
}