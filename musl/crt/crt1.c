#include <features.h>
#include "libc.h"

#define START "_start"

#include "crt_arch.h"

int main();
weak void _init();
weak void _fini();
int __libc_start_main(int (*)(), int, char **,
	void (*)(), void(*)(), void(*)());

void _start_c(long *p)
{
  int argc = 1;
  char *argv[] = {"enclave.signed.so", 0, 0, 0};
	__libc_start_main(main, argc, argv, _init, _fini, 0);
}
