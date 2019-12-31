#define SYS_eret 89 //ret to host
.globl EAPP_RETURN
EAPP_RETURN:
	li a7, SYS_eret
	ecall
