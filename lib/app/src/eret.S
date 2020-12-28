#define SYS_eret 89 //ret to host
#define SYS_acquire_enclave  87
#define SYS_call_enclave  86
#define SYS_enclave_return  85

.globl EAPP_RETURN
EAPP_RETURN:
	li a7, SYS_eret
	ecall

.globl EAPP_ACQUIRE_ENCLAVE
EAPP_ACQUIRE_ENCLAVE:
    li a7, SYS_acquire_enclave
    ecall
    ret

.globl EAPP_CALL_ENCLAVE
EAPP_CALL_ENCLAVE:
    li a7, SYS_call_enclave
    ecall
    ret

.globl SERVER_RETURN
SERVER_RETURN:
	li a7, SYS_enclave_return
	ecall
