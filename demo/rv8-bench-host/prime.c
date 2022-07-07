//#include "eapp.h"
//#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include "test.h"

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
  printf("%s is running\n", "Prime");
  unsigned long ret;
  ret = prime_loop(111);
  //EAPP_RETURN(ret);
}

int  main(){
  unsigned long begin_cycle, end_cycle;
  begin_cycle = dd_get_cycle();
  prime(NULL);
  end_cycle = dd_get_cycle();
  printf("[Prime] running takes (cycles): %lu\n", end_cycle-begin_cycle);
  return 0;
}
