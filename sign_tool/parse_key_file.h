#ifndef _PENGLAI_PARSE_KEY_FILE
#define _PENGLAI_PARSE_KEY_FILE

#include <assert.h>

int parse_priv_key_file(const char* priv_key_file, unsigned char* priv_key, unsigned char* pub_key);

int parse_pub_key_file(const char* pub_key_file, unsigned char* pub_key);

void parse_signature_DER(const char* sig_file, unsigned char* signature);

void generate_signature_DER(const char* sig_file, unsigned char* signature);

void generate_sm2_sig();

#endif