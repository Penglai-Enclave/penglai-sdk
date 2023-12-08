/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserve.

  This file contains symmetric encryption implementation with SM4 Block Cipher Algorithm,
  supporting Cipher Block Chaining(CBC) mode and Offset codebook(OCB) mode. CBC mode can
  assure confidentiality with simpler interfaces, while OCB mode, which support AEAD, can
  provide both message authentication and privacy.

  Macro definition:
    1.CBC_CipherTextLen                 // get CBC ciphertext length by plaintext length
    2.OCB_CipherTextLen                 // get OCB ciphertext length by plaintext length
  Function List:
    1.SM4_CBC_Encrypt                   // CBC encryption function
    2.SM4_CBC_Decrypt                   // CBC decryption function
    3.SM4_OCB_Encrypt                   // OCB encryption function
    4.SM4_OCB_Decrypt                   // OCB decryption function
**************************************************************************/

#pragma once

#define KEY_SIZE          16
#define IV_SIZE           16
#define BLOCK_SIZE        16

/**
 * get CBC mode ciphertext length by plaintext length, in CBC mode
 * the length of ciphertext will be a multiple of the block size.
 * @param n                   plaintext length
 */
#define CBC_CipherText_Len(n)  (((n) + (BLOCK_SIZE) - 1) / (BLOCK_SIZE)) * BLOCK_SIZE

/**
 * Encrypts a message with associated data.
 * @param Key                 128 bit encryption key.
 * @param IV                  IV, can be a counter, don't use the
 * same nonce for a key with different message/associated data.
 * @param PlainText           Data to be encrypted.
 * @param PlainTextLen        Data length in bytes.
 * @param CipherText          Encrypted output
 * @param CipherTextLen       Length of pre allocated 'CipherText'
 * buffer, used to check if space is enough.
 * @return                    Size of ciphertext in byte after encrypt.
 */
int SM4_CBC_Encrypt(const unsigned char *Key, const unsigned char *IV, const unsigned char PlainText[],
    int PlainTextLen, unsigned char CipherText[], int CipherTextLen);

/**
 * Encrypts a message with associated data.
 * @param Key                 128 bit encryption key.
 * @param IV                  IV, can be a counter, don't use the
 * same nonce for a key with different message/associated data.
 * @param CipherText          CipherText to be decrypted.
 * @param CipherTextLen       CipherText length in bytes.
 * @param PlainText           Decrypted output
 * @param PlainTextLen        Length of pre allocated 'PlainText'
 * buffer, used to check if space is enough.
 * @return                    Size of plaintext in byte after decrypt.
 */
int SM4_CBC_Decrypt(const unsigned char *Key, const unsigned char *IV, const unsigned char CipherText[],
    int CipherTextLen, unsigned char PlainText[], int PlainTextLen);

#define OCB_TAG_SIZE      16
/**
 * get OCB mode ciphertext length by plaintext length, in OCB mode
 * the length of ciphertext is plaintext length.
 * @param n                   plaintext length
 */
#define OCB_CipherText_Len(n)  (n)

/**
 * Encrypts a message with associated data.
 * @param key                    128 bit encryption key.
 * @param nonce                  IV, can be a counter, don't use the
 * same nonce for a key with different message/associated data.
 * @param nonce_length           A trivial parameter about nonce
 * length in bytes.
 * @param message                Data to be encrypted.
 * @param message_length         Data length in bytes.
 * @param associated_data        See the README.md for this.
 * @param associated_data_length Associated Data length in bytes.
 * @param out                    output with length [message_length + 16 bytes]
 */
void SM4_OCB_Encrypt(const unsigned char key[16], const unsigned char nonce[16], unsigned int nonce_length,
    const unsigned char *message, unsigned int message_length, const unsigned char *associated_data,
    int associated_data_length, unsigned char *out);

/**
 * Decrypts a message with associated data.
 * @param key                    128 bit encryption key.
 * @param nonce                  The IV used with the encryption function
 * @param nonce_length           A trivial parameter about nonce
 * length in bytes.
 * @param encrypted              Encrypted data (aka ciphertext), with
 * the 16-byte authentication tag appended to it.
 * @param encrypted_length       Ciphertext length in bytes, excluding
 * the 16-byte authentication tag.
 * @param associated_data        See the README.md for this.
 * @param associated_data_length Associated Data length in bytes.
 * @param out                    output with length [encrypted_length]
 * @return                       MUST BE CHECKED. Zero if decipher succesful.
 */
int SM4_OCB_Decrypt(const unsigned char key[16], const unsigned char nonce[16], unsigned int nonce_length,
    const unsigned char *encrypted, unsigned int encrypted_length, const unsigned char *associated_data,
    int associated_data_length, unsigned char *out);
