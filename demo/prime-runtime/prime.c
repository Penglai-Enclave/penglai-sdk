#include "eapp.h"
#include "print.h"
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
  eapp_print("%s is running\n", "Prime");
  unsigned long ret;
  ret = prime_loop(111);
  return 0;
}

int EAPP_ENTRY main(){
  unsigned long * args;
  int ret;
  EAPP_RESERVE_REG;
	unsigned long begin_cycle, end_cycle;
	begin_cycle = dd_get_cycle();
  ret = prime(args);
	end_cycle = dd_get_cycle();
	eapp_print("[Prime] Running takes (cycles): %lu\n", end_cycle-begin_cycle);
  EAPP_RETURN(end_cycle-begin_cycle);
}
