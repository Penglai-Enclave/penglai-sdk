#define SYS_eret 99 //ret to host
#define SYS_ocall 98 //OCALL
#define SYS_get_key 88

.globl EAPP_RETURN
EAPP_RETURN:
	li a6, SYS_eret
	li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	#li a7, SYS_eret
	ecall

.globl EAPP_OCALL
EAPP_OCALL:
	li a6, SYS_ocall
	li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	ecall
	ret

.globl EAPP_GET_KEY
EAPP_GET_KEY:
	li a6, SYS_get_key
    li a7, 0x100101 //SBI_EXT_PENGLAI_ENCLAVE
	ecall
    ret
