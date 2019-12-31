#include "eapp.h"
#include <stdlib.h>
#include <stdio.h>

int mem(unsigned long * args)
{
  char *ptr, *ptr2;
  int i;
  ptr = (char*)malloc(0x100);
  ptr2 = (char*)malloc(0x100);
  for(i = 0; i < 10; i++)
  {
    ptr[i] = 1;
  }
  for(i = 0; i < 10; i++)
  {
    ptr2[i] = 1;
  }

  free(ptr2);
  ptr2 = (char*)malloc(0x100);
  free(ptr);
  free(ptr2);

  EAPP_RETURN(ptr);
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  mem(args);
}
