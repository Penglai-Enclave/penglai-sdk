#define SYS_eret 99 //ret to host
#define SYS_ocall 98 //OCALL

.globl EAPP_RETURN
EAPP_RETURN:
	li a6, SYS_eret
	li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	#li a7, SYS_eret
	ecall

.globl EAPP_OCALL
EAPP_OCALL:
	li a6, SYS_eret
	li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	ecall
    ret
