#include "eapp.h"

int loop(unsigned long * args)
{
  unsigned long ret = 0;
  volatile int count = 0;
  while (1){
	  count++;
  }
  EAPP_RETURN(ret);
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  loop(args);
}
