#include "elf.h"

void elf_args_init(struct elf_args* elf_args, char *filename)
{
  int fs;
  struct stat stat_buf;
  elf_args->fd = open(filename, O_RDONLY);
  if(elf_args->fd < 0)
  {
    fprintf(stderr,"LIB: the elf file is not existed\n");
    return;
  }

  fs = fstat(elf_args->fd, &stat_buf);
  if(fs != 0)
  {
    fprintf(stderr,"LIB: fstat is failed \n");
    return;
  }

  elf_args->size = stat_buf.st_size;
  if(elf_args->size <= 0)
  {
    fprintf(stderr,"LIB: the elf file'size  is zero\n");
    return;
  }

  elf_args->ptr = mmap(NULL, elf_args->size, PROT_READ, MAP_PRIVATE, elf_args->fd, 0);
  if (elf_args->ptr == MAP_FAILED)
  {
    fprintf(stderr, "LIB: can not mmap enough memory for elf file");
    return;
  }

  return;
}

void elf_args_destroy(struct elf_args* elf_args)
{
  close(elf_args->fd);
  munmap(elf_args->ptr, elf_args->size);
}

bool elf_valid(struct elf_args* elf_args)
{
  return (elf_args->fd >= 0 && elf_args->size > 0 && elf_args->ptr != NULL);
}
