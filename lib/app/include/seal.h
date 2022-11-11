#ifndef _SEAL_H
#define _SEAL_H

#include <stdint.h>
#include "SM4_Enc.h"

#define SEAL_KEY_SALT_LEN       16
#define SEAL_KEY_LEN            KEY_SIZE
#define SEAL_DATA_NONCE_LEN     IV_SIZE
#define SEAL_DATA_TAG_LEN       OCB_TAG_SIZE

/*
 * Payload_data Layout:
 *   | - - Encrypted text - - | - - tag - - | - - additional mac text - - |
 * Payload_data Size:
 *   txt_encrypt_size + SEAL_DATA_TAG_LEN + add_mac_txt_size
 */
typedef struct _sealed_data_t {
    uint8_t nonce[SEAL_DATA_NONCE_LEN];
    uint8_t salt[SEAL_KEY_SALT_LEN];
    uint32_t txt_encrypt_size;
    uint32_t add_mac_txt_size;
    uint8_t payload_data[];
} penglai_sealed_data_t;

int DeriveSealingKey(const uint8_t *salt, int salt_len, uint8_t *seal_key_buf, int seal_key_len);

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
uint32_t penglai_calc_sealed_data_size(const uint32_t add_mac_txt_size, const uint32_t txt_encrypt_size);

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
uint32_t penglai_get_add_mac_txt_len(const penglai_sealed_data_t* p_sealed_data);

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
uint32_t penglai_get_encrypt_txt_len(const penglai_sealed_data_t* p_sealed_data);

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
    penglai_sealed_data_t *p_sealed_data);

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
    uint32_t *p_decrypted_text_length);


#endif