#include "ocall.h"
#include "eapp.h"

unsigned long acquire_enclave(char* name)
{
    unsigned long retval = 0;
    retval = EAPP_ACQUIRE_ENCLAVE(name);
    return retval;
}

unsigned long call_enclave(unsigned long handle, struct call_enclave_arg_t *arg)
{
    unsigned long retval = 0;
    retval = EAPP_CALL_ENCLAVE(handle, arg);
    return retval;
}
