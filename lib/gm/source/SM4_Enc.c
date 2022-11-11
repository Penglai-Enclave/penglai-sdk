/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserve.

  This file contains symmetric encryption implementation with SM4 Block Cipher Algorithm,
  supporting Cipher Block Chaining(CBC) mode and Offset codebook(OCB) mode.

  Function List:
    1.SM4_CBC_Encrypt                       //CBC mode encryption function
    2.SM4_CBC_Decrypt                       //CBC mode decryption function
    3.SM4_OCB_Encrypt                       //OCB mode encryption function
    4.SM4_OCB_Decrypt                       //OCB mode decryption function
**************************************************************************/

#include <string.h>

#include "SM4_Enc.h"
#include "SM4.h"

#define ocb_ntz(a) __builtin_ctz((unsigned int)a)
#define ocb_ntz_round(a) ((a) == 0) ? 0 : (sizeof(unsigned int) * 8 - __builtin_clz((unsigned int)(a)) - 1)
#define ocb_memcpy(a, b, c) __builtin_memcpy(a, b, c)

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
int SM4_CBC_Encrypt(const unsigned char *Key, const unsigned char *IV,
                    const unsigned char PlainText[], int PlainTextLen, unsigned char CipherText[], int CipherTextLen)
{
    int rounds;
    int start, end;
    unsigned char input[16];
    unsigned char ciphertext[16];
    unsigned char plaintext[16];
    int output_cursor = 0;

    if (CipherTextLen < CBC_CipherText_Len(PlainTextLen))
        return 0;

    rounds = CBC_CipherText_Len(PlainTextLen) / 16;
    for (int j = 0; j < rounds; ++j)
    {
        start = j * 16;
        end = (start + 16 > PlainTextLen) ? PlainTextLen : (start + 16);
        memset(plaintext, 0, 16);
        memcpy(plaintext, PlainText + start, end - start);

        for (int i = 0; i < 16; ++i)
        {
            if (j == 0)
            {
                input[i] = plaintext[i] ^ IV[i];
            }
            else
            {
                input[i] = plaintext[i] ^ ciphertext[i];
            }
        }
        SM4_Encrypt(Key, input, ciphertext);
        memcpy(CipherText + output_cursor, ciphertext, 16);
        output_cursor += 16;
    }
    return output_cursor;
}

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
int SM4_CBC_Decrypt(const unsigned char *Key, const unsigned char *IV,
                    const unsigned char CipherText[], int CipherTextLen, unsigned char PlainText[], int PlainTextLen)
{
    int rounds;
    int start, end;
    unsigned char ciphertext[16];
    unsigned char input[16];
    unsigned char output[16];
    unsigned char plaintext[16];
    int output_cursor = 0;

    if (CipherTextLen % 16 != 0)
        return 0;

    rounds = CipherTextLen / 16;
    for (int j = 0; j < rounds; j++)
    {
        start = j * 16;
        end = (start + 16 > PlainTextLen) ? PlainTextLen : (start + 16);
        memcpy(ciphertext, CipherText + start, 16);

        SM4_Decrypt(Key, ciphertext, output);
        for (int i = 0; i < 16; ++i)
        {
            if (j == 0)
            {
                plaintext[i] = IV[i] ^ output[i];
            }
            else
            {
                plaintext[i] = input[i] ^ output[i];
            }
        }

        for (int k = 0; k < end - start; ++k)
        {
            PlainText[output_cursor++] = plaintext[k];
        }
        memcpy(input, ciphertext, 16);
    }
    return output_cursor;
}

static void double_arr(unsigned char s[16])
{
    const unsigned char first_bit = -(s[0] >> 7);
    for (int i = 0; i < 15; i++)
    {
        s[i] &= 127;
        s[i] <<= 1;
        s[i] |= s[i + 1] >> 7;
    }
    s[15] &= 127;
    s[15] <<= 1;
    s[15] ^= first_bit & 135;
}

static inline void xor_16(unsigned char *__restrict a, const unsigned char *__restrict b)
{
    for (int i = 0; i < 16; i++)
        a[i] ^= b[i];
}

static void hash(const unsigned char *key, const unsigned char *associated_data, int associated_data_length,
                 const unsigned char l[][16], const unsigned char *l_asterisk,
                 unsigned char *out)
{
    const unsigned int m = associated_data_length / 16;
    unsigned char offset[16] = {0};
    unsigned char cipher_temp[16];

    for (int i = 0; i < 16; i++)
        out[i] = 0;

    for (int i = 0; i < m; i++)
    {
        for (int k = 0; k < 16; k++)
            cipher_temp[k] = associated_data[i * 16 + k];
        xor_16(offset, l[ocb_ntz(i + 1)]);
        xor_16(cipher_temp, offset);
        SM4_Encrypt(key, cipher_temp, cipher_temp);
        xor_16(out, cipher_temp);
    }

    const unsigned int a_asterisk_length = (unsigned int)(associated_data_length % 16);
    const unsigned int full_block_length = associated_data_length ^ a_asterisk_length;
    if (a_asterisk_length > 0)
    {
        xor_16(offset, l_asterisk);
        for (unsigned int i = 0; i < a_asterisk_length; i++)
            cipher_temp[i] = associated_data[full_block_length + i];
        cipher_temp[a_asterisk_length] = 0x80;
        for (int i = a_asterisk_length + 1; i < 16; i++)
            cipher_temp[i] = 0;
        xor_16(cipher_temp, offset);
        SM4_Encrypt(key, cipher_temp, cipher_temp);
        xor_16(out, cipher_temp);
    }
}

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
void SM4_OCB_Encrypt(const unsigned char *key, const unsigned char *nonce, unsigned int nonce_length,
                     const unsigned char *message, unsigned int message_length, const unsigned char *associated_data, int associated_data_length,
                     unsigned char *out)
{
    const unsigned int m = message_length / 16;
    const unsigned int l_length =
        (message_length > associated_data_length) ? (ocb_ntz_round(m) + 1) : (ocb_ntz_round(associated_data_length / 16) + 1);
    unsigned char l[l_length][16];
    unsigned char l_asterisk[16] = {0};
    unsigned char l_dollar[16];

    SM4_Encrypt(key, l_asterisk, l_asterisk);
    // L_* ^^
    for (int i = 0; i < 16; i++)
        l[0][i] = l_asterisk[i];
    double_arr(l[0]);
    for (int i = 0; i < 16; i++)
        l_dollar[i] = l[0][i];
    double_arr(l[0]);
    // L_0 ^^^
    for (unsigned int i = 1; i < l_length; i++)
    {
        for (int k = 0; k < 16; k++)
            l[i][k] = l[i - 1][k];
        double_arr(l[i]);
    }
    unsigned char offset[24] = {0};
    int index = 15 - nonce_length;
    offset[index++] |= 1;
    for (unsigned int i = 0; i < nonce_length; index++, i++)
        offset[index] = nonce[i];
    unsigned int bottom = offset[15] % 64;
    offset[15] ^= bottom;
    SM4_Encrypt(key, offset, offset);
    for (int i = 0; i < 8; i++)
        offset[16 + i] = offset[i];
    for (int i = 0; i < 8; i++)
        offset[16 + i] ^= offset[i + 1];

    const unsigned int shift = bottom / 8;
    const unsigned int bit_shift = bottom % 8;
    for (int i = 0; i < 16; i++)
        offset[i] = ((offset[i + shift] << bit_shift) | (offset[i + shift + 1] >> (8 - bit_shift))) & 255;

    ocb_memcpy(out, message, message_length);
    for (int i = 0; i < m; i++)
    {
        xor_16(offset, l[ocb_ntz(i + 1)]);
        xor_16(&out[i * 16], offset);
        SM4_Encrypt(key, &out[i * 16], &out[i * 16]);
        xor_16(&out[i * 16], offset);
    }

    const unsigned int p_asterisk_length = (unsigned int)(message_length % 16);
    const unsigned int full_block_length = message_length ^ p_asterisk_length;
    unsigned char checksum[16] = {0};
    for (unsigned int i = 0; i < full_block_length; i++)
        checksum[i % 16] ^= message[i];

    if (p_asterisk_length > 0)
    {
        xor_16(offset, l_asterisk);
        for (int i = 0; i < 16; i++)
            out[full_block_length + i] = offset[i];
        SM4_Encrypt(key, &out[full_block_length], &out[full_block_length]);
        // ^^pad
        for (unsigned int i = 0; i < p_asterisk_length; i++)
            out[full_block_length + i] ^= message[full_block_length + i];
        for (unsigned int i = 0; i < p_asterisk_length; i++)
            checksum[i] ^= message[full_block_length + i];
        checksum[p_asterisk_length] ^= 0x80;
    }
    xor_16(checksum, offset);
    xor_16(checksum, l_dollar);
    SM4_Encrypt(key, checksum, checksum);
    hash(key, associated_data, associated_data_length, l, l_asterisk, offset);
    xor_16(checksum, offset);
    for (int i = 0; i < 16; i++)
        out[full_block_length + p_asterisk_length + i] = checksum[i];
}

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
int SM4_OCB_Decrypt(const unsigned char *key, const unsigned char *nonce, unsigned int nonce_length,
                    const unsigned char *encrypted, unsigned int encrypted_length, const unsigned char *associated_data, int associated_data_length,
                    unsigned char *out)
{
    const unsigned int m = encrypted_length / 16;
    const unsigned int l_length =
        (encrypted_length > associated_data_length) ? (ocb_ntz_round(m) + 1) : (ocb_ntz_round(associated_data_length / 16) + 1);
    unsigned char l[l_length][16];
    unsigned char l_asterisk[16] = {0};
    unsigned char l_dollar[16];

    SM4_Encrypt(key, l_asterisk, l_asterisk);
    // L_* ^^
    for (int i = 0; i < 16; i++)
        l[0][i] = l_asterisk[i];
    double_arr(l[0]);
    for (int i = 0; i < 16; i++)
        l_dollar[i] = l[0][i];
    double_arr(l[0]);
    // L_0 ^^^
    for (unsigned int i = 1; i < l_length; i++)
    {
        for (int k = 0; k < 16; k++)
            l[i][k] = l[i - 1][k];
        double_arr(l[i]);
    }
    unsigned char offset[24] = {0};
    int index = 15 - nonce_length;
    offset[index++] |= 1;
    for (unsigned int i = 0; i < nonce_length; index++, i++)
        offset[index] = nonce[i];
    unsigned int bottom = offset[15] % 64;
    offset[15] ^= bottom;
    SM4_Encrypt(key, offset, offset);
    for (int i = 0; i < 8; i++)
        offset[16 + i] = offset[i];
    for (int i = 0; i < 8; i++)
        offset[16 + i] ^= offset[i + 1];

    const unsigned int shift = bottom / 8;
    const unsigned int bit_shift = bottom % 8;
    for (int i = 0; i < 16; i++)
        offset[i] = ((offset[i + shift] << bit_shift) | (offset[i + shift + 1] >> (8 - bit_shift))) & 255;

    const unsigned int c_asterisk_length = (unsigned int)(encrypted_length % 16);
    const unsigned int full_block_length = encrypted_length ^ c_asterisk_length;
    ocb_memcpy(out, encrypted, full_block_length);
    for (int i = 0; i < m; i++)
    {
        xor_16(offset, l[ocb_ntz(i + 1)]);
        xor_16(&out[i * 16], offset);
        SM4_Decrypt(key, &out[i * 16], &out[i * 16]);
        xor_16(&out[i * 16], offset);
    }

    unsigned char checksum[16] = {0};
    for (unsigned int i = 0; i < full_block_length; i++)
        checksum[i % 16] ^= out[i];

    if (c_asterisk_length > 0)
    {
        xor_16(offset, l_asterisk);
        unsigned char pad[16];
        for (int i = 0; i < 16; i++)
            pad[i] = offset[i];
        SM4_Encrypt(key, pad, pad);
        // ^^pad
        for (unsigned int i = 0; i < c_asterisk_length; i++)
            pad[i] ^= encrypted[full_block_length + i];
        for (unsigned int i = 0; i < c_asterisk_length; i++)
            out[full_block_length + i] = pad[i];
        // ^^p_asterisk
        for (unsigned int i = 0; i < c_asterisk_length; i++)
            checksum[i] ^= pad[i];
        checksum[c_asterisk_length] ^= 0x80;
    }
    xor_16(checksum, offset);
    xor_16(checksum, l_dollar);
    SM4_Encrypt(key, checksum, checksum);
    hash(key, associated_data, associated_data_length, l, l_asterisk, offset);
    xor_16(checksum, offset);
    xor_16(checksum, &encrypted[encrypted_length]);
    unsigned char diff = 0;
    for (unsigned int i = 0; i < 16; i++)
        diff |= checksum[i];
    return (unsigned int)diff;
}
