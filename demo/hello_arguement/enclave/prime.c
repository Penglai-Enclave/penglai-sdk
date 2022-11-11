#include "arguements.h"
#include "eapp.h"
#include "print.h"
#include "ocall.h"

#include <string.h>

/* ENTRY POINT main() */
int EAPP_ENTRY main(){
    unsigned long * args;
    EAPP_RESERVE_REG;

    eapp_print("Penglai Enclave is running\n");

    get_name_t arguement_protected;
    char name[] = "Alice's enclave";
    
    if(sizeof(get_name_t) > DEFAULT_UNTRUSTED_SIZE){
        eapp_print("Size to copy is larger than untrusted mem size \n");
        EAPP_RETURN(0);
    }

    // get arguements from untrusted mem to safe memory
    memcpy(&arguement_protected, (const void *)DEFAULT_UNTRUSTED_PTR, sizeof(get_name_t));
    eapp_print("argue A: %d, argue B:%d\n", arguement_protected.A, arguement_protected.B);

    // enclave handle arguements
    arguement_protected.C = arguement_protected.A + arguement_protected.B;
    memset(arguement_protected.enclave_name, 0, sizeof(arguement_protected.enclave_name));
    memcpy(&arguement_protected.enclave_name, name, sizeof(name));

    // trans output
    memcpy((void *)DEFAULT_UNTRUSTED_PTR, &arguement_protected, sizeof(get_name_t));

    EAPP_RETURN(0);
}
