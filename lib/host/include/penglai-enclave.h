#ifndef _PL_ENCLAVE
#define _PL_ENCLAVE
#include "elf.h"
#include "param.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdarg.h>

#define PENGLAI_ENCLAVE_DEV_PATH "/dev/penglai_enclave_dev"

struct PLenclave
{
  struct elf_args *elffile;
  int eid;
  int fd;
  struct  penglai_enclave_user_param user_param;
};

void PLenclave_init(struct PLenclave *PLenclave);
void PLenclave_finalize(struct PLenclave *PLenclave);
int PLenclave_create(struct PLenclave* PLenclave, struct elf_args* u_elffile, struct enclave_args* u_param);
int PLenclave_run(struct PLenclave *PLenclave);
int PLenclave_stop(struct PLenclave *PLenclave);
int PLenclave_resume(struct PLenclave *PLenclave);
int PLenclave_destroy(struct PLenclave *PLenclave);
int PLenclave_debug_print(struct PLenclave *PLenclave);



#endif
