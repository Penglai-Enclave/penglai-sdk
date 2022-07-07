#include "eapp.h"

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  EAPP_RETURN(0xdd);
}
