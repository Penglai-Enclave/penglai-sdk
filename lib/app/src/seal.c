#include "seal.h"
#include "Random.h"
#include "print.h"
#include "ocall.h"

#include <string.h>
#include <stddef.h>

extern unsigned long EAPP_GET_KEY(uintptr_t salt_ptr, uintptr_t salt_len, uintptr_t key_buf_ptr, uintptr_t key_buf_len);

int DeriveSealingKey(const uint8_t *salt, int salt_len, uint8_t *seal_key_buf, int seal_key_len){
    int ret;
    
    ret = EAPP_GET_KEY((uintptr_t)salt, salt_len, (uintptr_t)seal_key_buf, seal_key_len);
    if(ret != 0){
        eapp_print("[SDK] function DeriveSealingKey: EAPP_GET_KEY failed\n");
    }
    return ret;
}

/* penglai_calc_sealed_data_size
    * Purpose: This function is used to determine how much memory to allocate for penglai_sealed_data_t structure.
    *
    * Paramters:
    *      add_mac_txt_size - [IN] Length of the optional additional data stream in bytes
    *      txt_encrypt_size - [IN] Length of the data stream to be encrypted in bytes
    *
    * Return Value:
    *      uint32_t - The minimum number of bytes that need to be allocated for the penglai_sealed_data_t structure
    *      If the function fails, the return value is UINT32_MAX
*/
uint32_t penglai_calc_sealed_data_size(const uint32_t add_mac_txt_size, const uint32_t txt_encrypt_size){
    if (UINT32_MAX - add_mac_txt_size <= txt_encrypt_size) {
        return UINT32_MAX;
    }
    if (UINT32_MAX - (add_mac_txt_size + txt_encrypt_size) <= SEAL_DATA_TAG_LEN) {
        return UINT32_MAX;
    }
    if (UINT32_MAX - (add_mac_txt_size + txt_encrypt_size + SEAL_DATA_TAG_LEN) <= (uint32_t)sizeof(penglai_sealed_data_t)) {
        return UINT32_MAX;
    }
    return (uint32_t)(add_mac_txt_size + txt_encrypt_size + SEAL_DATA_TAG_LEN + (uint32_t)sizeof(penglai_sealed_data_t));
}

/* penglai_get_add_mac_txt_len
    * Purpose: This function is used to determine how much memory to allocate for the additional_MAC_text buffer
    *
    * Parameter:
    *      p_sealed_data - [IN] Pointer to the penglai_sealed_data_t structure which was populated by the penglai_seal_data function
    *
    * Return Value:
    *      uint32_t - The number of bytes in the optional additional MAC buffer
    *      If the function fails, the return value is UINT32_MAX
*/
uint32_t penglai_get_add_mac_txt_len(const penglai_sealed_data_t* p_sealed_data){
    const penglai_sealed_data_t *tmp_sealed_data = (const penglai_sealed_data_t *)p_sealed_data;
    if (tmp_sealed_data == NULL) {
        return UINT32_MAX;
    }
    return tmp_sealed_data->add_mac_txt_size;
}

/* penglai_get_encrypt_txt_len
    * Purpose: This function is used to determine how much memory to allocate for the decrypted data returned by the penglai_unseal_data function
    *
    * Parameter:
    *      p_sealed_data - [IN] Pointer to the penglai_sealed_data_t structure which was populated by the penglai_seal_data function
    *
    * Return Value:
    *      uint32_t - The number of bytes in the encrypted data buffer
    *      If the function fails, the return value is UINT32_MAX
*/
uint32_t penglai_get_encrypt_txt_len(const penglai_sealed_data_t* p_sealed_data){
    const penglai_sealed_data_t *tmp_sealed_data = (const penglai_sealed_data_t *)p_sealed_data;
    if (tmp_sealed_data == NULL) {
        return UINT32_MAX;
    }
    return tmp_sealed_data->txt_encrypt_size;
}

static void data_copy(penglai_sealed_data_t *sealed_data, uint8_t *salt, uint8_t *nonce,
                     const uint8_t *mac_data, uint8_t mac_data_len)
{
    uint32_t mac_data_offset;
    memcpy(sealed_data->salt, salt, SEAL_KEY_SALT_LEN);
    memcpy(sealed_data->nonce, nonce, SEAL_DATA_NONCE_LEN);
    mac_data_offset = sealed_data->txt_encrypt_size + SEAL_DATA_TAG_LEN;
    if (mac_data != NULL && mac_data_len != 0) {
        memcpy(&(sealed_data->payload_data[mac_data_offset]), mac_data, mac_data_len);
    }
    sealed_data->add_mac_txt_size = mac_data_len;
    return;
}

/* penglai_seal_data
    * Purpose: This algorithm is used to SM4-OCB encrypt the input data.  Specifically,
    *          two input data sets can be provided, one is the text to encrypt (p_text2encrypt)
    *          the second being optional additional text that should not be encrypted but will
    *          be part of the OCB MAC calculation.
    *          The penglai_sealed_data_t structure should be allocated prior to the API call and
    *          should include buffer storage for the MAC text and encrypted text.
    *          The penglai_sealed_data_t structure contains the data required to unseal the data on
    *          the same system it was sealed.
    *
    * Parameters:
    *      additional_MACtext_length - [IN] length of the plaintext data stream in bytes
    *                                  The additional data is optional and thus the length
    *                                  can be zero if no data is provided
    *      p_additional_MACtext - [IN] pointer to the plaintext data stream to be GCM protected
    *                             The additional data is optional. You may pass a NULL pointer
    *                             but additional_MACtext_length must be zero in that case
    *      text2encrypt_length - [IN] length of the data stream to encrypt in bytes
    *      p_text2encrypt - [IN] pointer to data stream to encrypt
    *      sealed_data_size - [IN] Size of the sealed data buffer passed in
    *      p_sealed_data - [OUT] pointer to the sealed data structure containing protected data
    *
    * Return Value:
    *      MUST BE CHECKED. Return 0 if seal successfully. Return -1 if seal failed.
*/
int penglai_seal_data(const uint32_t additional_MACtext_length,
    const uint8_t *p_additional_MACtext,
    const uint32_t text2encrypt_length,
    const uint8_t *p_text2encrypt,
    const uint32_t sealed_data_size,
    penglai_sealed_data_t *p_sealed_data){

    uint8_t key_buf[SEAL_KEY_LEN];
    uint8_t salt[SEAL_KEY_SALT_LEN];
    uint8_t nonce[SEAL_DATA_NONCE_LEN];
    int result;

    if(penglai_calc_sealed_data_size(additional_MACtext_length, text2encrypt_length) > sealed_data_size){
        eapp_print("sealed_data_size is too small.\n");
        return -1;
    }

    penglai_read_rand(salt, SEAL_KEY_SALT_LEN);
    result = DeriveSealingKey((const uint8_t *)salt, SEAL_KEY_SALT_LEN, key_buf, SEAL_KEY_LEN);
    if (result != 0) {
        eapp_print("DeriveTARootKey failed\n");
        return -1;
    }
    
    penglai_read_rand(nonce, SEAL_DATA_NONCE_LEN);
    SM4_OCB_Encrypt(key_buf, nonce, SEAL_DATA_NONCE_LEN, p_text2encrypt, text2encrypt_length,
        p_additional_MACtext, additional_MACtext_length, p_sealed_data->payload_data);
    
    p_sealed_data->txt_encrypt_size = text2encrypt_length;
    data_copy(p_sealed_data, salt, nonce, p_additional_MACtext, additional_MACtext_length);

    memset(nonce, 0, SEAL_DATA_NONCE_LEN);
    memset(salt, 0, SEAL_KEY_SALT_LEN);
    memset(key_buf, 0, SEAL_KEY_LEN);
    return result;
}

/* penglai_unseal_data
    * Purpose: Unseal the sealed data structure passed in and populate the MAC text and decrypted text
    *          buffers with the appropriate data from the sealed data structure.
    *
    * Parameters:
    *      p_sealed_data - [IN] pointer to the sealed data structure containing protected data
    *      p_additional_MACtext - [OUT] pointer to the plaintext data stream which was GCM protected
    *                             The additiona data is optional. You may pass a NULL pointer but
    *                             p_additional_MACtext_length must be zero in that case
    *      p_additional_MACtext_length - [IN/OUT] pointer to length of the plaintext data stream in bytes
    *                             If there is not additional data, this parameter should be zero.
    *      p_decrypted_text - [OUT] pointer to decrypted data stream
    *      p_decrypted_text_length - [IN/OUT] pointer to length of the decrypted data stream to encrypt in bytes
    *
    * Return Value:
    *      MUST BE CHECKED. Return 0 if authentication passed and unseal successfully. Return -1 if unseal failed.
*/
int penglai_unseal_data(const penglai_sealed_data_t *p_sealed_data,
    uint8_t *p_additional_MACtext,
    uint32_t *p_additional_MACtext_length,
    uint8_t *p_decrypted_text,
    uint32_t *p_decrypted_text_length){
    
    uint8_t key_buf[SEAL_KEY_LEN];
    uint32_t mac_data_offset;
    int result;

    if(penglai_get_encrypt_txt_len(p_sealed_data) > *p_decrypted_text_length){
        eapp_print("decrypted_text_length is too small.\n");
        return -1;
    }
    if(penglai_get_add_mac_txt_len(p_sealed_data) > *p_additional_MACtext_length){
        eapp_print("additional_MACtext_length is too small.\n");
        return -1;
    }
 
    result = DeriveSealingKey((const uint8_t *)p_sealed_data->salt, SEAL_KEY_SALT_LEN, key_buf, SEAL_KEY_LEN);
    if (result != 0) {
        eapp_print("DeriveTARootKey failed\n");
        goto error;
    }

    mac_data_offset = p_sealed_data->txt_encrypt_size + SEAL_DATA_TAG_LEN;
    result = SM4_OCB_Decrypt(key_buf, p_sealed_data->nonce, SEAL_DATA_NONCE_LEN, p_sealed_data->payload_data,
        p_sealed_data->txt_encrypt_size, &(p_sealed_data->payload_data[mac_data_offset]),
        p_sealed_data->add_mac_txt_size, p_decrypted_text);
    if(result != 0){
        eapp_print("[penglai_unseal_data] Fail Assenticate, Data integrity is compromised.\n");
    }

    memcpy(p_additional_MACtext, &p_sealed_data->payload_data[mac_data_offset], p_sealed_data->add_mac_txt_size);
    *p_additional_MACtext_length = p_sealed_data->add_mac_txt_size;
    *p_decrypted_text_length = p_sealed_data->txt_encrypt_size;

error:
    memset(key_buf, 0, SEAL_KEY_LEN);
    return result;
}
