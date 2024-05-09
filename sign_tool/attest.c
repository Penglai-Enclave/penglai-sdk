#include "attest.h"
// #include "gm/sm3.h"
// #include "gm/sm2.h"
#include "SM3.h"
#include "SM2_sv.h"
#include <string.h>
#include <stdio.h>

void print_level(int level){
  // static int counter = 0;
  // if(counter > 100){
  //   return;
  // }
  printf("level: %d\n", level);
}

// void print_sm3_state(struct sm3_context *hash_ctx, char *add_info){
//   static int counter = 0;
//   unsigned char output[32];
//   sm3_state_print(hash_ctx, output);

//   // if(counter > 100){
//   //   return;
//   // }
//   printf("Line %d: %s: ", counter++, add_info);
// 	for (int i = 0; i < 32; i++) {
// 		printf("0x%02X ", output[i]);
// 	}
//   printf("\n");
// }

static int hash_enclave_mem(SM3_STATE *hash_ctx, pt_entry_t* ptes, int level, uintptr_t va, int hash_va)
{
  uintptr_t pte_per_page = RISCV_PGSIZE/sizeof(pt_entry_t);
  pt_entry_t *pte;
  uintptr_t i = 0;
  int hash_curr_va = hash_va;

  //should never happen
  if(level <= 0)
    return 1;

  // print_level(level);

  for(pte = ptes, i = 0; i < pte_per_page; pte += 1, i += 1)
  {
    if(!(*pte & PTE_V))
    {
      if(hash_curr_va == 0){
        // printf("[hash_curr_va] this page isn't valid, index: %d, pte: 0x%02x\n", (int)i, *((unsigned char*)pte));
      }
      hash_curr_va = 1;
      continue;
    }

    uintptr_t curr_va = 0;
    if(level == ((VA_BITS - RISCV_PGSHIFT) / RISCV_PGLEVEL_BITS))
      curr_va = (uintptr_t)(-1UL << VA_BITS) + (i << (VA_BITS - RISCV_PGLEVEL_BITS));
    else
      curr_va = va + (i << ((level-1) * RISCV_PGLEVEL_BITS + RISCV_PGSHIFT));
    uintptr_t pa = (*pte >> PTE_PPN_SHIFT) << RISCV_PGSHIFT;

    //found leaf pte
    if((*pte & PTE_R) || (*pte & PTE_X))
    {
      if(hash_curr_va)
      {
        // printf("[hash_curr_va] last level va: 0x%08x%08x, curr_va: 0x%08x%08x, index: %d\n", *((int*)&va+1), *((int*)&va), *((int*)&curr_va+1), *((int*)&curr_va), (int)i);
        SM3_process(hash_ctx, (unsigned char*)&curr_va, sizeof(uintptr_t));
        // print_sm3_state(hash_ctx, "curr_va");
        //update hash with  page attribution
        // printf("[hash_curr_va] pte: 0x%02x\n", *((unsigned char*)pte));
        SM3_process(hash_ctx, (unsigned char*)pte, 1);
        // print_sm3_state(hash_ctx, "pte_cof");
        hash_curr_va = 0;
      }

      //4K page
      if(level == 1)
      {
        SM3_process(hash_ctx, (void*)pa, 1 << RISCV_PGSHIFT);
        // print_sm3_state(hash_ctx, "4k_page");
      }
      //2M page
      else if(level == 2)
      {
        SM3_process(hash_ctx, (void*)pa, 1 << (RISCV_PGSHIFT + RISCV_PGLEVEL_BITS));
        // print_sm3_state(hash_ctx, "2M_page");
      }
    }
    else
    {
      hash_curr_va = hash_enclave_mem(hash_ctx, (pt_entry_t*)pa, level - 1, curr_va, hash_curr_va);
    }
  }

  // print_level(level + 1);

  return hash_curr_va;
}

void hash_enclave(unsigned long entry_point, enclave_mem_t* enclave_mem, void* hash, uintptr_t nonce_arg, uintptr_t untrusted_ptr,
                  uintptr_t untrusted_size, uintptr_t kbuffer_ptr, uintptr_t kbuffer_size)
{
  SM3_STATE hash_ctx;
  uintptr_t nonce = nonce_arg;

  SM3_init(&hash_ctx);

  SM3_process(&hash_ctx, (unsigned char*)(&entry_point), sizeof(unsigned long));
//   print_sm3_state(&hash_ctx, "entry_p");
    // configuration parameters
  SM3_process(&hash_ctx, (unsigned char*)(&(untrusted_ptr)),
        sizeof(unsigned long));
  SM3_process(&hash_ctx, (unsigned char*)(&(untrusted_size)),
        sizeof(unsigned long));
  SM3_process(&hash_ctx, (unsigned char*)(&(kbuffer_ptr)),
        sizeof(unsigned long));
  SM3_process(&hash_ctx, (unsigned char*)(&(kbuffer_size)),
        sizeof(unsigned long));
  hash_enclave_mem(&hash_ctx, enclave_mem->enclave_root_pt,
      (VA_BITS - RISCV_PGSHIFT) / RISCV_PGLEVEL_BITS, 0, 1);
    printf("%lx %lx %lx %lx\n", untrusted_ptr, untrusted_size, kbuffer_ptr, kbuffer_size);

  SM3_process(&hash_ctx, (unsigned char*)(&nonce), sizeof(uintptr_t));
//   print_sm3_state(&hash_ctx, "nonce 0");

  SM3_done(&hash_ctx, hash);
}

void sign_enclave(struct signature_t* signature, unsigned char *message, int len, unsigned char *prikey_arg)
{
    struct prikey_t *prikey = (struct prikey_t *)prikey_arg;
    SM2_Sign(message, len, prikey->dA, (unsigned char *)(signature->r),
        (unsigned char *)(signature->s));
}

int verify_enclave(struct signature_t* signature, unsigned char *message, int len, unsigned char *pubkey_arg)
{
    int ret = 0;
    struct pubkey_t *pubkey = (struct pubkey_t *)pubkey_arg;
    ret = SM2_Verify(message, len, pubkey->xA, pubkey->yA,
        (unsigned char *)(signature->r), (unsigned char *)(signature->s));
    return ret;
}
