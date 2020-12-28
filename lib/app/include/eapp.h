#ifndef  _EAPP_COMMON
#define _EAPP_COMMON

/***********************/
/*                     */
/*                     */
/***********************/  /*0x1000000000 + 2*4096*/
/*    untrusted mem    */
/***********************/  /*0x1000000000*/
/*                     */
/***********************/  /*brk + 16*4096*/
/*        heap         */
/***********************/  /*brk*/
/*      code & data    */
/***********************/  /*0x1000*/

struct call_enclave_arg_t
{
    unsigned long req_arg;
    unsigned long req_vaddr;
    unsigned long req_size;
    unsigned long resp_val;
    unsigned long resp_vaddr;
    unsigned long resp_size;
};

void EAPP_RETURN(unsigned long rval) __attribute__((noreturn));
void SERVER_RETURN(struct call_enclave_arg_t *arg) __attribute__((noreturn));

unsigned long EAPP_ACQUIRE_ENCLAVE(char* name);
unsigned long EAPP_CALL_ENCLAVE(unsigned long handle, struct call_enclave_arg_t *arg);

unsigned long acquire_enclave(char* name);
unsigned long call_enclave(unsigned long handle, struct call_enclave_arg_t* arg);

#define EAPP_ENTRY __attribute__((__section__(".text._start")))
#define EAPP_RESERVE_REG   asm volatile("addi sp,sp,-256\n\t" \
                                                                                            "sd ra, 1*8(sp)\n\t" \
                                                                                            "sd gp, 3*8(sp)\n\t" \
                                                                                            "sd tp, 4*8(sp)\n\t" \
                                                                                            "sd t0, 5*8(sp)\n\t" \
                                                                                            "sd t1, 6*8(sp)\n\t" \
                                                                                            "sd t2, 7*8(sp)\n\t" \
                                                                                            "sd s0, 8*8(sp)\n\t" \
                                                                                            "sd s1, 9*8(sp)\n\t" \
                                                                                            "sd a0, 10*8(sp)\n\t" \
                                                                                            "sd a1, 11*8(sp)\n\t" \
                                                                                            "sd a2, 12*8(sp)\n\t" \
                                                                                            "sd a3, 13*8(sp)\n\t" \
                                                                                            "sd a4, 14*8(sp)\n\t" \
                                                                                            "sd a5, 15*8(sp)\n\t" \
                                                                                            "sd a6, 16*8(sp)\n\t" \
                                                                                            "sd a7, 17*8(sp)\n\t" \
                                                                                            "sd s2, 18*8(sp)\n\t" \
                                                                                            "sd s3, 19*8(sp)\n\t" \
                                                                                            "sd s4, 20*8(sp)\n\t" \
                                                                                            "sd s5, 21*8(sp)\n\t" \
                                                                                            "sd s6, 22*8(sp)\n\t" \
                                                                                            "sd s7, 23*8(sp)\n\t" \
                                                                                            "sd s8, 24*8(sp)\n\t" \
                                                                                            "sd s9, 25*8(sp)\n\t" \
                                                                                            "sd s10, 26*8(sp)\n\t" \
                                                                                            "sd s11, 27*8(sp)\n\t" \
                                                                                            "sd t3, 28*8(sp)\n\t" \
                                                                                            "sd t4, 29*8(sp)\n\t" \
                                                                                            "sd t5, 30*8(sp)\n\t" \
                                                                                            "sd t6, 31*8(sp)\n\t" \
                                                                                            "mv %0 ,sp":"=r "(args) )
#endif
