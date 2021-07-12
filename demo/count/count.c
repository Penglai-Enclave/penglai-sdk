#include "eapp.h"

#define ITERATIONS 100000000
int count(unsigned long * args)
{
  unsigned long ret = 0;
  volatile int count = ITERATIONS;
  while (count--){
	  ret++;
  }
  EAPP_RETURN(ret);
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  count(args);
}
