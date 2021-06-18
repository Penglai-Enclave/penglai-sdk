#define SYS_eret 89 //ret to host
.globl EAPP_RETURN
EAPP_RETURN:
	li a6, SYS_eret
	li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	#li a7, SYS_eret
	ecall
