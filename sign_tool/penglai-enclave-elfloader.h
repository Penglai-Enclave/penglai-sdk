#ifndef _PENGLAI_ENCLAVE_ELFLOADER
#define _PENGLAI_ENCLAVE_ELFLOADER

#include <linux/elf.h>
#include "penglai-enclave-page.h"
#include "math.h"

#define PAGE_UP(addr)	(((addr)+((RISCV_PGSIZE)-1))&(~((RISCV_PGSIZE)-1)))
#define PAGE_DOWN(addr)	((addr)&(~((RISCV_PGSIZE)-1)))

int penglai_enclave_eapp_preprare(
		enclave_mem_t* enclave_mem,  
		void* elf_ptr, 
		unsigned long size, 
		vaddr_t * elf_entry_point, 
		vaddr_t stack_ptr, 
		int stack_size,
		unsigned long *meta_offset,
		unsigned long *meta_blocksize);

int penglai_enclave_elfmemsize(void* elf_ptr, int* size);

int alloc_umem(unsigned long untrusted_mem_size, unsigned long* untrusted_mem_ptr, vaddr_t vaddr, enclave_mem_t* enclave_mem);

int alloc_kbuffer(unsigned long kbuffer_size, unsigned long* kbuffer_ptr, vaddr_t vaddr, enclave_mem_t* enclave_mem);

#endif
