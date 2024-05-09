#include "penglai-enclave-elfloader.h"

#define	ROUND_TO(x, align)  (((x) + ((align)-1)) & ~((align)-1))

int penglai_enclave_load_NOBITS_section(enclave_mem_t* enclave_mem, void * elf_sect_addr, int elf_sect_size)
{
	vaddr_t addr;
	vaddr_t enclave_new_page;
	int size;
	for(addr = (vaddr_t)elf_sect_addr; addr < (vaddr_t)elf_sect_addr + elf_sect_size; addr += RISCV_PGSIZE)
	{
		enclave_new_page = enclave_alloc_page(enclave_mem, addr, ENCLAVE_USER_PAGE);
		if (addr + RISCV_PGSIZE >(vaddr_t) elf_sect_addr + elf_sect_size)
			size = elf_sect_size % RISCV_PGSIZE;
		else
			size = RISCV_PGSIZE;
		// printf("enclave_new_page paddr: 0x%08x%08x, set 0 size: %08x\n", *((int*)&enclave_new_page+1), *((int*)&enclave_new_page), size);
		memset((void *) enclave_new_page, 0, size);
	}
	return 0;
}

/* elf_prog_infile_addr @ content in elf file
   elf_prog_addr @ virtual addr for program begin addr
   elf_prog_size @ size of prog segment
   */
int penglai_enclave_load_program(enclave_mem_t* enclave_mem, vaddr_t elf_prog_infile_addr, void * elf_prog_addr, int elf_prog_size)
{
	vaddr_t addr;
	vaddr_t enclave_new_page;
	int size;
	for(addr =  (vaddr_t)elf_prog_addr; addr <  (vaddr_t)elf_prog_addr + elf_prog_size; addr += RISCV_PGSIZE)
	{

		enclave_new_page = enclave_alloc_page(enclave_mem, addr, ENCLAVE_USER_PAGE);
		if (addr + RISCV_PGSIZE > (vaddr_t)elf_prog_addr + elf_prog_size)
			size = elf_prog_size % RISCV_PGSIZE;
		else
			size = RISCV_PGSIZE;
		memcpy((void* )enclave_new_page, (void *)(elf_prog_infile_addr + addr - (vaddr_t)elf_prog_addr), size);
	}
	return 0;
}

int get_meta_property(void* elf_ptr, unsigned long size, unsigned long *meta_offset, unsigned long *meta_blocksize){
	Elf64_Ehdr elf_hdr;
	Elf64_Shdr* shdr;
	int i;
	bool found = false;

	memcpy(&elf_hdr, elf_ptr, sizeof(Elf64_Ehdr));
	shdr = (Elf64_Shdr *)((vaddr_t)elf_ptr + elf_hdr.e_shoff);
	const char *shstrtab = (char *)(elf_ptr + (shdr + elf_hdr.e_shstrndx)->sh_offset);

	/* Loader section */
	for (i = 0; i < elf_hdr.e_shnum; i++, shdr++)
	{
		printf("section [%u] %s: sh_addr = %x, sh_size = %x, sh_offset = %x, sh_name = %x\n", 
			i, shstrtab + shdr->sh_name, shdr->sh_addr, shdr->sh_size, shdr->sh_offset, shdr->sh_name);
		if (!strcmp(shstrtab + shdr->sh_name, ".note.penglaimeta"))
		{
			found = true;
			break;
		}
	}
	if (found == false)
    {
        printf("ERROR: The enclave image should have '.note.penglaimeta' section\n");
        return -1;
    }
	/* We require that enclaves should have .note.penglaimeta section to store the metadata information
     * We limit this section is used for metadata only and ISV should not extend this section.
     *
     * .note.penglaimeta layout:
     *
     * |  namesz         |
     * |  metadata size  |
     * |  type           |
     * |  name           |
     * |  metadata       |
     */

    Elf64_Nhdr* note = (elf_ptr + shdr->sh_offset);
    if (note == NULL)
	{
		printf("ERROR: Nhdr is NULL\n");
		return -1;
	}

    if (shdr->sh_size != ROUND_TO(sizeof(Elf64_Nhdr) + note->n_namesz + note->n_descsz, shdr->sh_addralign ))
    {
        printf("ERROR: The '.note.penglaimeta' section size is not correct.\n");
        return -1;
    }
    
    const char * meta_name = "penglai_metadata";
    if (note->n_namesz != (strlen(meta_name)+1) || memcmp((void *)(elf_ptr + shdr->sh_offset + sizeof(Elf64_Nhdr)), meta_name, note->n_namesz))
    {
        printf("ERROR: The note in the '.note.penglaimeta' section must be named as \"penglai_metadata\"\n");
        return -1;
    }

    *meta_offset = (unsigned long)(shdr->sh_offset + sizeof(Elf64_Nhdr) + note->n_namesz);
    *meta_blocksize = note->n_descsz;

	printf("success! meta_offset: %d, meta_blocksize: %d\n", *meta_offset, *meta_blocksize);
    return true;
}

/* ptr @ user pointer
   hdr @ kernel pointer
   */
int penglai_enclave_loadelf(enclave_mem_t*enclave_mem, void* elf_ptr, unsigned long size, vaddr_t * elf_entry_point)
{
	Elf64_Ehdr elf_hdr;
	Elf64_Phdr elf_prog_hdr;
	Elf64_Shdr elf_sect_hdr;
	int i,  elf_prog_size;
	vaddr_t elf_sect_ptr, elf_prog_ptr, elf_prog_addr, elf_prog_infile_addr;
	memcpy(&elf_hdr, elf_ptr, sizeof(Elf64_Ehdr));
	
	*elf_entry_point = elf_hdr.e_entry;
	elf_sect_ptr = (vaddr_t) elf_ptr + elf_hdr.e_shoff;

	/* Loader section */
	for (i = 0; i < elf_hdr.e_shnum;i++)
	{
		memcpy(&elf_sect_hdr,(void *)elf_sect_ptr,sizeof(Elf64_Shdr));
		
		if (elf_sect_hdr.sh_addr == 0)
		{
			elf_sect_ptr += sizeof(Elf64_Shdr);
			continue;
		}

		/* Load NOBITS section */
		if (elf_sect_hdr.sh_type == SHT_NOBITS)
		{
			vaddr_t elf_sect_addr = elf_sect_hdr.sh_addr;
			int elf_sect_size = elf_sect_hdr.sh_size;
			printf("[penglai_enclave_loadelf] Load NOBITS section: sh_addr: 0x%08x%08x, sh_size: %d\n",*((int*)&elf_sect_addr+1), *((int*)&elf_sect_addr), elf_sect_size);
			if (penglai_enclave_load_NOBITS_section(enclave_mem,(void *)elf_sect_addr,elf_sect_size) < 0)
			{
				printf("KERNEL MODULE: penglai enclave load NOBITS  section failed\n");
				return -1;
			}
		}
		elf_sect_ptr += sizeof(Elf64_Shdr);
	}

	/* Load program segment */
	elf_prog_ptr = (vaddr_t) elf_ptr + elf_hdr.e_phoff;

	for(i = 0; i < elf_hdr.e_phnum;i++)
	{
		memcpy(&elf_prog_hdr,(void *)elf_prog_ptr,sizeof(Elf64_Phdr));

		/* Virtual addr for program begin address */
		elf_prog_addr = elf_prog_hdr.p_vaddr;
		elf_prog_size = elf_prog_hdr.p_filesz;
		elf_prog_infile_addr = (vaddr_t) elf_ptr + elf_prog_hdr.p_offset;
		printf("[penglai_enclave_loadelf] Load program segment: prog_addr: 0x%08x%08x, prog_size: %d\n",*((int*)&elf_prog_addr+1), *((int*)&elf_prog_addr), elf_prog_size);
		if (penglai_enclave_load_program(enclave_mem, elf_prog_infile_addr, (void *)elf_prog_addr, elf_prog_size) < 0)
		{
			printf("KERNEL MODULE: penglai enclave load program failed\n");
			return -1;
		}
		printf("[penglai_enclave_loadelf] elf_prog_addr:0x%lx elf_prog_size:0x%x, infile_addr:0x%lx\n",
				elf_prog_addr, elf_prog_size, elf_prog_infile_addr);
		elf_prog_ptr += sizeof(Elf64_Phdr);
	}
	return 0;
}

int penglai_enclave_eapp_preprare(enclave_mem_t* enclave_mem,  void* elf_ptr, unsigned long size, vaddr_t * elf_entry_point, vaddr_t stack_ptr, int stack_size, unsigned long *meta_offset, unsigned long *meta_blocksize)
{
	vaddr_t addr;

	/* Init stack */
	for(addr = stack_ptr - stack_size; addr < stack_ptr; addr += RISCV_PGSIZE)
	{
		enclave_alloc_page(enclave_mem, addr, ENCLAVE_STACK_PAGE);
	}

	/* Load elf file */
	if(penglai_enclave_loadelf(enclave_mem, elf_ptr, size, elf_entry_point) < 0)
	{
		printf("KERNEL MODULE: penglai enclave loadelf failed\n");
		return -1;
	}

	if(get_meta_property(elf_ptr, size, meta_offset, meta_blocksize) < 0){
		printf("KERNEL MODULE: penglai enclave get metadata property failed\n");
		return -1;
	}

	return 0;
}

int penglai_enclave_elfmemsize(void* elf_ptr, int* size)
{
	Elf64_Ehdr elf_hdr;
	Elf64_Phdr elf_prog_hdr;
	Elf64_Shdr elf_sect_hdr;
	int i, elf_prog_size;
	vaddr_t elf_sect_ptr, elf_prog_ptr;
	memcpy(&elf_hdr, elf_ptr, sizeof(Elf64_Ehdr));

	elf_sect_ptr = (vaddr_t) elf_ptr + elf_hdr.e_shoff;

	for (i = 0; i < elf_hdr.e_shnum;i++)
	{
		memcpy(&elf_sect_hdr,(void *)elf_sect_ptr,sizeof(Elf64_Shdr));
		if (elf_sect_hdr.sh_addr == 0)
		{
			elf_sect_ptr += sizeof(Elf64_Shdr);
			continue;
		}

		// Calculate the size of the NOBITS section
		if (elf_sect_hdr.sh_type == SHT_NOBITS)
		{
			int elf_sect_size = elf_sect_hdr.sh_size;
			*size = *size + elf_sect_size;
		}
		elf_sect_ptr += sizeof(Elf64_Shdr);
	}

	// Calculate the size of the PROGBITS segment
	elf_prog_ptr = (vaddr_t) elf_ptr + elf_hdr.e_phoff;

	for(i = 0; i < elf_hdr.e_phnum;i++)
	{
		memcpy(&elf_prog_hdr,(void *)elf_prog_ptr,sizeof(Elf64_Phdr));

		// Virtual addr for program begin address
		elf_prog_size = elf_prog_hdr.p_filesz;
		*size = *size + elf_prog_size;
		elf_prog_ptr += sizeof(Elf64_Phdr);
	}
	return 0;
}

int map_untrusted_mem(enclave_mem_t* enclave_mem, vaddr_t vaddr, paddr_t paddr, unsigned long size)
{
	vaddr_t addr = vaddr;

	for (; addr < vaddr + size; addr+=RISCV_PGSIZE) {
		map_va2pa(enclave_mem, addr, paddr, ENCLAVE_UNTRUSTED_PAGE);
		paddr += RISCV_PGSIZE;
	}
	return 0;
}

int map_kbuffer(enclave_mem_t* enclave_mem, vaddr_t vaddr, paddr_t paddr, unsigned long size)
{
	vaddr_t addr = vaddr;

	for (; addr < vaddr + size; addr += RISCV_PGSIZE) {
		map_va2pa(enclave_mem, addr, paddr, ENCLAVE_KBUFFER_PAGE);
		paddr += RISCV_PGSIZE;
	}
	return 0;
}

int alloc_umem(unsigned long untrusted_mem_size, unsigned long* untrusted_mem_ptr, vaddr_t vaddr, enclave_mem_t* enclave_mem)
{
	int ret = 0;
	char* addr = (char*)malloc(untrusted_mem_size + RISCV_PGSIZE);
	if(!addr)
	{
		printf("SIGN_TOOL: can not alloc untrusted mem \n");
		return -1;
	}

    vaddr_t page_addr = (vaddr_t)PAGE_UP((unsigned long)addr);
    memset((void*)page_addr, 0, untrusted_mem_size);
	*untrusted_mem_ptr = page_addr;
	map_untrusted_mem(enclave_mem, vaddr, page_addr, untrusted_mem_size);

	return ret;
}

int alloc_kbuffer(unsigned long kbuffer_size, unsigned long* kbuffer_ptr, vaddr_t vaddr, enclave_mem_t* enclave_mem)
{
	int ret = 0;
    kbuffer_size = 0x1 << (ilog2(kbuffer_size - 1) + 1);
    char* addr = (char*)malloc(kbuffer_size + RISCV_PGSIZE);
	if(!addr)
	{
		printf("SIGN_TOOL: can not alloc untrusted mem \n");
		return -1;
	}

    vaddr_t page_addr = (vaddr_t)PAGE_UP((unsigned long)addr);
    memset((void*)page_addr, 0, kbuffer_size);
	*kbuffer_ptr = page_addr;
	map_kbuffer(enclave_mem, vaddr, page_addr, kbuffer_size);

	return ret;
}