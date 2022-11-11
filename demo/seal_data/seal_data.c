#include "eapp.h"
#include "print.h"
#include "seal.h"
#include <string.h>

uint8_t seal_data[] = "Data to encrypt, Hello penglai";
uint8_t additional_text[] = "add mac text";

void printHex(unsigned char *c, int n)
{
	int i, rounds;
    rounds = n / 16;
	for (i = 0; i < rounds; i++) {
		eapp_print("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                    c[i*16], c[i*16+1], c[i*16+2], c[i*16+3],
                    c[i*16+4], c[i*16+5], c[i*16+6], c[i*16+7],
                    c[i*16+8], c[i*16+9], c[i*16+10], c[i*16+11],
                    c[i*16+12], c[i*16+13], c[i*16+14], c[i*16+15]);
	}
}

int seal_test_func(unsigned long * args)
{
    int ret;
    // test 1: Can DeriveSealingKey() derive the same key on same salt between multiple setup times?
    eapp_print("[TEST 1]: Can DeriveSealingKey() derive the same key on same salt between multiple setup times?\n");
    uint8_t salt[SEAL_KEY_SALT_LEN] = {
            0x2d, 0xfb, 0x42, 0x9a, 0x48, 0x69, 0x7c, 0x34,
            0x00, 0x6d, 0xa8, 0x86, 0x9a, 0xf6, 0xd0, 0x12};
    uint8_t seal_key[SEAL_KEY_LEN];
    ret = DeriveSealingKey(salt, SEAL_KEY_SALT_LEN, seal_key, SEAL_KEY_LEN);
    if(ret != 0)
        eapp_print("DeriveSealingKey() failed\n");
    printHex(seal_key, SEAL_KEY_LEN);

    // test 2: If change salt, can DeriveSealingKey() derive different keys?
    eapp_print("[TEST 2]: If change salt, can DeriveSealingKey() derive different keys?\n");
    salt[0] = 0xff;
    ret = DeriveSealingKey(salt, SEAL_KEY_SALT_LEN, seal_key, SEAL_KEY_LEN);
    if(ret != 0)
        eapp_print("DeriveSealingKey() failed\n");
    printHex(seal_key, SEAL_KEY_LEN);

    // test 3: Seal output cypher text and unseal can get the same plaintext
    eapp_print("[TEST 3]: Can seal output cypher text and unseal get the correct plaintext\n");
    eapp_print("plaintext: %s, add_text: %s\n", seal_data, additional_text);
    /******** prepare data to seal ***********/
    uint32_t data_len = strlen((const char *)seal_data);
    uint32_t add_len = strlen((const char *)additional_text);
    uint32_t sealed_data_len = penglai_calc_sealed_data_size(add_len, data_len);
    if (sealed_data_len == UINT32_MAX){
        eapp_print("seal_data_len is too large, ERROR_OUT_OF_MEMORY\n");
        return -1;
    }
    uint8_t sealed_data_buffer[sealed_data_len];
    penglai_sealed_data_t *sealed_data = (penglai_sealed_data_t *)sealed_data_buffer;
    
    ret = penglai_seal_data((const uint32_t)add_len, (const uint8_t *)additional_text,
                (const uint32_t)data_len, (const uint8_t *)seal_data,
                (const uint32_t)sealed_data_len, sealed_data);
    if (ret == 0){
        eapp_print("seal_data encrypt success\n");
    } else {
        eapp_print("seal_data encrypt failed\n");
        return -1;
    }

    /******** prepare to unseal data ***********/
    uint32_t encrypt_add_len = penglai_get_add_mac_txt_len(sealed_data);
    uint32_t encrypt_data_len = penglai_get_encrypt_txt_len(sealed_data);
    uint8_t demac_data[encrypt_add_len];
    uint8_t decrypted_seal_data[encrypt_data_len];
    /* can pass authenticate check? */
    ret = penglai_unseal_data((const penglai_sealed_data_t *)sealed_data, demac_data,
                &encrypt_add_len, decrypted_seal_data, &encrypt_data_len);
    if(ret == 0){
        eapp_print("seal_data decrypt authenticate Success\n");
    } else {
        eapp_print("seal_data decrypt authenticate Fail\n");
        return -1;
    }
    /* Are add_text and plaintext same with init */
    if(encrypt_add_len != add_len || strncmp((const char *)demac_data, (const char *)additional_text, add_len) != 0) {
        eapp_print("demac_data from sealed_data is not equal to initial additional_text\n");
        return -1;
    }
    if(encrypt_data_len != data_len || strncmp((const char *)decrypted_seal_data, (const char *)seal_data, data_len) != 0) {
        eapp_print("decrypted_data from sealed_data is not equal to initial seal_data\n");
        return -1;
    }
    eapp_print("penglai_seal test finished --- ALL TEST SUCCESS!\n");
    return ret;
}

int EAPP_ENTRY main(){
  unsigned long * args;
  EAPP_RESERVE_REG;
  unsigned long ret = seal_test_func(args);
  EAPP_RETURN((unsigned long)ret);
}
