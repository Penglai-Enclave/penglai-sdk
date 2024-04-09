#define __SYSCALL_LL_E(x) (x)
#define __SYSCALL_LL_O(x) (x)
#define UNTRUSTED_MEM_PTR 0x0000001000000000
#define DEFAULT_BRK_SIZE 4096 * 32
#define DEFAULT_MMAP_SIZE 4096 * 64
extern char __brk_start0;
extern char __mmap_start0;
#ifdef USE_TLSF
#include "tlsf.h"
extern int g_mem_pool_inited;
extern tlsf_t g_tlsf_pool;
extern char __mmap_end;
#define MMAP_START ((char *)&__mmap_start0)
#define TOTAL_MMAP_SIZE ((char *)&__mmap_end - (char *)&__mmap_start0)
#endif

#ifdef CUSTOM_BRK_SIZE
static unsigned long brk_size = CUSTOM_BRK_SIZE;
#else
static unsigned long brk_size = DEFAULT_BRK_SIZE;
#endif

#ifdef CUSTOM_MMAP_SIZE
static unsigned long mmap_size = CUSTOM_MMAP_SIZE;
#else
static unsigned long mmap_size = DEFAULT_MMAP_SIZE;
#endif

#define __asm_syscall(...) \
	__asm__ __volatile__ ("ecall\n\t" \
	: "=r"(a0) : __VA_ARGS__ : "memory"); \
	return a0; \

// #define __asm_syscall(...) \
// 	__asm__ __volatile__ ("ecall\n\t" \
// 	: "=r"(a0) : __VA_ARGS__ : "memory"); 

struct mmap_metadata
{
  char type;
  unsigned long size;
};

#define SYS_eret 99 //ret to host
#define SBI_EXT_PENGLAI_ENCLAVE 0x100101
/**
 * When the eapp calls a syscall that is not yet supported, 
 * it simply exits the enclave by __asm_exit_enclave
*/
#define __asm_exit_enclave() \
    __asm__ __volatile__ ( \
        "li a6, %0\n\t" \
        "li a7, %1\n\t" \
        "ecall\n\t" \
        : /* No output */ \
        : "i"(SYS_eret), "i"(SBI_EXT_PENGLAI_ENCLAVE)\
        : "memory", "a6", "a7" \
    );

static inline long __syscall0(long n)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0");
  __asm_exit_enclave();
	__asm_syscall("r"(a7))
}

static inline long __syscall1(long n, long a)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
  static unsigned long brk_offset = 4096 * 8;
	switch(n)
 	{
    case SYS_brk:
      if(a0 == 0)
      {
          // __asm_syscall("r"(a7), "0"(a0))
        return (unsigned long)((&__brk_start0) + brk_offset);
      }
      else
      {
        unsigned long retval = a0;
        brk_offset = a0 - (unsigned long)&__brk_start0;
        if(brk_offset >= 2 * brk_size)
          return -1;
        // __asm_syscall("r"(a7), "0"(a0))
        return retval;
      }
    default:
        __asm_exit_enclave();
  }
  
  __asm_syscall("r"(a7), "0"(a0))
}

static inline long __syscall2(long n, long a, long b)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
	register long a1 __asm__("a1") = b;
	switch(n)
  {
    case SYS_munmap:
      {
#ifdef USE_TLSF
        tlsf_free(g_tlsf_pool, (void *)a0);
#else
        struct mmap_metadata* tmp;
        tmp = (struct mmap_metadata* )(a - sizeof(struct mmap_metadata));
        tmp->type = 1;
#endif
        return 0;
      }
    default:
        __asm_exit_enclave();
  }
  __asm_syscall("r"(a7), "0"(a0), "r"(a1))
}

static inline long __syscall3(long n, long a, long b, long c)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
	register long a1 __asm__("a1") = b;
	register long a2 __asm__("a2") = c;
  __asm_exit_enclave();
	__asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2))
}

static inline long __syscall4(long n, long a, long b, long c, long d)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
	register long a1 __asm__("a1") = b;
	register long a2 __asm__("a2") = c;
	register long a3 __asm__("a3") = d;
  switch(n)
 	{
    case SYS_write:
      {
	    // __asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2), "r"(a3))
        return 0;
      }
      break;
    default:
        __asm_exit_enclave();
  }
	__asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2), "r"(a3))
}

static inline long __syscall5(long n, long a, long b, long c, long d, long e)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
	register long a1 __asm__("a1") = b;
	register long a2 __asm__("a2") = c;
	register long a3 __asm__("a3") = d;
	register long a4 __asm__("a4") = e;

  __asm_exit_enclave();
	__asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4))
}

static inline long __syscall6(long n, long a, long b, long c, long d, long e, long f)
{
	register long a7 __asm__("a7") = n;
	register long a0 __asm__("a0") = a;
	register long a1 __asm__("a1") = b;
	register long a2 __asm__("a2") = c;
	register long a3 __asm__("a3") = d;
	register long a4 __asm__("a4") = e;
	register long a5 __asm__("a5") = f;
  struct mmap_metadata* tmp;
  unsigned long ret;
  unsigned long __mmap_size0 = 0;
  switch(n)
  {
    case SYS_mmap:
#ifdef USE_TLSF
        if (!g_mem_pool_inited) {
            g_tlsf_pool = tlsf_create_with_pool(MMAP_START, TOTAL_MMAP_SIZE);
            g_mem_pool_inited = 1;
        }
        return (long)tlsf_malloc(g_tlsf_pool, a1);
#else
      tmp =(struct mmap_metadata*)((unsigned long)&__mmap_start0 + __mmap_size0);
      while(tmp)
      {
        if((unsigned long)tmp < (unsigned long)&__mmap_start0 + mmap_size)
        {
          if((tmp->type == 0) || ((tmp->type == 1) && (tmp->size >= a1 + sizeof(struct mmap_metadata)) ) )
          {
            unsigned long tmp_size = tmp->size;
            unsigned char tmp_type = tmp->type;
            tmp->type = 2;
            tmp->size = a1;
            ret = (unsigned long)&__mmap_start0 + __mmap_size0 + sizeof(struct mmap_metadata); 
            __mmap_size0 += sizeof(struct mmap_metadata) + a1;
            tmp =(struct mmap_metadata*)((unsigned long)&__mmap_start0 + __mmap_size0);
            if((unsigned long)tmp >= (unsigned long)&__mmap_start0 + mmap_size)
            {
              ret -= sizeof(struct mmap_metadata);
              ((struct mmap_metadata*)ret)->type = tmp_type;
              ((struct mmap_metadata*)ret)->size = tmp_size;
              return -1;
            }
            tmp->type = tmp_type;
            if(tmp->size == 0)
              tmp->size = 0;
            else
              tmp->size = tmp_size - a1 - sizeof(struct mmap_metadata); 
            // __asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5))
            return ret;
          }
          else if(((tmp->type == 1) && (tmp->size < a1 + sizeof(struct mmap_metadata)) ) && 
                        ((tmp->type == 1) && (tmp->size >= a1 )))
          {
            tmp->type = 2;
            ret = (unsigned long)&__mmap_start0 + __mmap_size0 + sizeof(struct mmap_metadata); 
            return ret;
          }
          else
          {  
            __mmap_size0 += sizeof(struct mmap_metadata) + tmp->size;
            tmp = (struct mmap_metadata *)((unsigned long)&__mmap_start0 + __mmap_size0);
          }
        }
        else
        {
          return -1;
        }
      }
      break;
#endif
    default:
        __asm_exit_enclave();
  }
	__asm_syscall("r"(a7), "0"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5))
}

#define VDSO_USEFUL
/* We don't have a clock_gettime function.
#define VDSO_CGT_SYM "__vdso_clock_gettime"
#define VDSO_CGT_VER "LINUX_2.6" */

#define IPC_64 0
