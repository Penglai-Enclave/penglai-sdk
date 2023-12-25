#ifndef _ELF
#define _ELF
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

struct elf_args
{
    int fd;
    unsigned long size;
    void * ptr;
};

void elf_args_init(struct elf_args* elf_args, char *filename);
void elf_args_destroy(struct elf_args* elf_args);
bool elf_valid(struct elf_args* elf_args);

#ifdef __cplusplus
}
#endif

#endif
