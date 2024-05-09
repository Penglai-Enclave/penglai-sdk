#include "penglai-enclave-page.h"

bool free_mem_empty(free_mem_t* free_mem){
	if(free_mem->tail == NULL){
		return true;
	}
	return false;
}

void free_mem_add_tail(struct free_mem_page* free_mem_page, free_mem_t* free_mem){
	if(free_mem_empty(free_mem)){
		free_mem->head = free_mem->tail = free_mem_page;
	} else {
		free_mem->tail->next = free_mem_page;
		free_mem->tail = free_mem_page;
	}
}

struct free_mem_page* free_mem_resume_head(free_mem_t* free_mem){
	struct free_mem_page* page;
	if(free_mem->tail == NULL){
		printf("free_mem: tail is NULL!!\n");
	}
	if(free_mem_empty(free_mem) || free_mem->head == NULL){
		printf("free_mem: No empty mem in enclave\n");
		return NULL;
	}
	page = free_mem->head;
	if(page->next == NULL){
		free_mem->head = free_mem->tail = NULL;
	} else {
		free_mem->head = page->next;
	}
	return page;
}

vaddr_t get_free_mem(free_mem_t* free_mem)
{
	struct free_mem_page* page;
	vaddr_t vaddr;

	page = free_mem_resume_head(free_mem);
	if(page == NULL){
		printf("KERNEL MODULE: get_free_mem: No empty mem in enclave\n");
		return 0;
	}
	vaddr = page->vaddr;
	/* Free the free_mem_t struct */
	free(page);

	return vaddr;
}

static void put_free_page(free_mem_t* free_mem, vaddr_t vaddr)
{
	struct free_mem_page* page = malloc(sizeof(struct free_mem_page));
	page->vaddr = vaddr;
	page->next = NULL;
	free_mem_add_tail(page, free_mem);

	return;
}

void init_free_mem(free_mem_t* free_mem, vaddr_t base, unsigned int count)
{
	vaddr_t cur;
	int i;
	cur = base;
	// printf("init_free_mem: count: %d\n", count);
	for(i = 0; i < count; i++)
	{
		put_free_page(free_mem, cur);
		cur += RISCV_PGSIZE;
	}

	return;
}

int clean_free_mem(free_mem_t* free_mem)
{
	struct free_mem_page* page;
	while(!free_mem_empty(free_mem))
	{
		page = free_mem_resume_head(free_mem);
		/* Free the free_mem_t struct */
		free(page);
	}

	return 0;
}

static inline pt_entry_t ptd_create(vaddr_t vaddr)
{
	unsigned long ppn = vaddr >> RISCV_PGSHIFT;
	return (ppn << PTE_PPN_SHIFT) | PTE_V;
}

static inline paddr_t pte2va(pt_entry_t pte)
{
	return (pte >> RISCV_PTE_PERMBITS) << RISCV_PT_SHIFT;
}

static inline int get_pt_index(vaddr_t vaddr, int level)
{
	int index = vaddr >> (VA_BITS - (level + 1)*RISCV_PGLEVEL_BITS);

	return index & ((1 << RISCV_PGLEVEL_BITS) - 1) ;
}

static inline int create_ptd_page(free_mem_t* free_mem, pt_entry_t* pte)
{
	vaddr_t addr = get_free_mem(free_mem);
	// printf("new page table: 0x%08x%08x\n", *((int*)&addr+1), *((int*)&addr));
	if(addr == 0){
		return -1;
	}
	*pte = ptd_create(addr) | PTE_V;

	return 0;
}

static pt_entry_t * walk_enclave_pt(free_mem_t* free_mem, pt_entry_t* enclave_root_pt, vaddr_t vaddr, bool create)
{
	pt_entry_t* pgdir = enclave_root_pt;
	int i;
	// static int counter = 0;

	for(i = 0; i < RISCV_PT_LEVEL-1; i++)
	{
		int pt_index = get_pt_index(vaddr, i);
		// if(counter < 20)
		// printf("walk_enclave_pt: level: %d, index: %d, ", i, pt_index);
		pt_entry_t pt_entry = pgdir[pt_index];
		// printf("pte_addr: %p", &pgdir[pt_index]);
		if(!(pt_entry & PTE_V))
		{
			// printf(" --not valid\n");
			if(create)
			{
				if(create_ptd_page(free_mem, &pgdir[pt_index]) < 0)
					return NULL;
				else
					pt_entry = pgdir[pt_index];
			}
			else
				printf("KERNEL MODULE: walk_enclave_pt fault\n");
		}
		// else{
		// 	printf(" --valid\n");
		// }
		pgdir = (pt_entry_t*)pte2va(pt_entry);
	}

	// printf("walk_enclave_pt: level: %d, index: %d\n", 2, get_pt_index(vaddr, RISCV_PT_LEVEL-1));
	return &pgdir[get_pt_index(vaddr, RISCV_PT_LEVEL-1)];
}

static inline pt_entry_t* clear_enclave_pt(pt_entry_t * enclave_root_pt, vaddr_t vaddr)
{
	pt_entry_t * pgdir = enclave_root_pt;
	int i;
	for (i = 0; i < RISCV_PT_LEVEL -1 ; i++)
	{
		int pt_index = get_pt_index(vaddr , i);
		pt_entry_t pt_entry = pgdir[pt_index];
		if(!(pt_entry & PTE_V))
		{
			return 0;
		}
		pgdir = (pt_entry_t *)pte2va(pt_entry);
	}
	pgdir[get_pt_index(vaddr , RISCV_PT_LEVEL - 1)] = 0;

	return &pgdir[get_pt_index(vaddr , RISCV_PT_LEVEL - 1)];
}

vaddr_t enclave_alloc_page(enclave_mem_t*enclave_mem, vaddr_t vaddr, unsigned long flags)
{
	vaddr_t free_page = get_free_mem(enclave_mem->free_mem);
	pt_entry_t *pte = walk_enclave_pt(enclave_mem->free_mem, enclave_mem -> enclave_root_pt, vaddr, true);
	*pte = ptd_create(free_page) | flags | PTE_V;

	return free_page;
}

vaddr_t map_va2pa(enclave_mem_t* enclave_mem, vaddr_t vaddr, paddr_t paddr, unsigned long flags)
{
	pt_entry_t *pte = walk_enclave_pt(enclave_mem->free_mem, enclave_mem -> enclave_root_pt, vaddr, true);
	*pte = ptd_create(paddr) | flags | PTE_V;
	return vaddr;
}

void enclave_mem_int(enclave_mem_t* enclave_mem, vaddr_t vaddr, int size, paddr_t paddr)
{
	pt_entry_t *pte;
	enclave_mem->free_mem = (free_mem_t*)malloc(sizeof(free_mem_t));
	enclave_mem->free_mem->head = NULL;
	enclave_mem->free_mem->tail = NULL;

	init_free_mem(enclave_mem->free_mem, vaddr, size / RISCV_PGSIZE);
	enclave_mem -> vaddr = vaddr;
	enclave_mem -> paddr = paddr;
	enclave_mem -> size = size;
	pte = (pt_entry_t *)get_free_mem(enclave_mem->free_mem);
	enclave_mem -> enclave_root_pt = pte;
	/*
	FIXME: create two special pages in enclave(the record for dynamic allocation pages)
	*/
	get_free_mem(enclave_mem->free_mem);
}

int enclave_mem_destroy(enclave_mem_t * enclave_mem)
{
	clean_free_mem(enclave_mem->free_mem);
	/*
	FIXME: clear two special pages in enclave(the record for dynamic allocation pages)
	Need to reclaim enclave mem to kernel ?
	free_pages(enclave->addr, enclave->size)
	free_pages(enclave dynamic alloc pages, size)
	*/
	return 0;
}
