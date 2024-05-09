#include <stdio.h>
#include <stdlib.h>
#include "penglai-enclave.h"
#include "param.h"
#include "penglai-enclave-elfloader.h"
#include "attest.h"
#include "riscv64.h"
#include "util.h"
#include "parse_key_file.h"

#define DEFAULT_CLOCK_DELAY 100000
#define STACK_POINT 0x0000004000000000
#define DEFAULT_UNTRUSTED_PTR   0x0000001000000000
#define ENCLAVE_DEFAULT_KBUFFER_SIZE              0x1000UL
#define ENCLAVE_DEFAULT_KBUFFER         0xffffffe000000000UL
#define MD_SIZE 64
#define MAX_ELF_SIZE 512*1024*1024
#define MAX_STACK_SIZE 64*1024*1024
#define MAX_UNTRUSTED_MEM_SIZE 256*1024

typedef enum _file_path_t
{
    ELF = 0,
    KEY = 1,
    OUTPUT,
    SIG,
    UNSIGNED,
    DUMPFILE
} file_path_t;

unsigned int total_enclave_page(int elf_size, int stack_size)
{
	unsigned int total_pages;
	total_pages = PAGE_UP(elf_size) / RISCV_PGSIZE + PAGE_UP(stack_size) / RISCV_PGSIZE + 15;
	return total_pages;
}

void init_enclave_user_param(struct penglai_enclave_user_param* user_param, struct elf_args* enclaveFile){
    struct enclave_args* params = malloc(sizeof(struct enclave_args));
    enclave_param_init(params);
    params->untrusted_mem_size = DEFAULT_UNTRUSTED_SIZE;
    params->untrusted_mem_ptr = 0;
    user_param->elf_ptr = (unsigned long)enclaveFile->ptr;
    user_param->elf_size = enclaveFile->size;
    user_param->stack_size = params->stack_size;
    user_param->untrusted_mem_ptr = params->untrusted_mem_ptr;
    user_param->untrusted_mem_size = params->untrusted_mem_size;
    user_param->ocall_buf_size = 0;
    user_param->resume_type = 0;
    free(params);
    return;
}

int penglai_enclave_create(struct penglai_enclave_user_param* enclave_param, enclave_css_t* enclave_css, unsigned long* meta_offset_arg)
{
	void *elf_ptr = (void*)enclave_param->elf_ptr;
	int elf_size = 0;
	if(penglai_enclave_elfmemsize(elf_ptr, &elf_size) < 0)
	{
		printf("SIGN_TOOL: calculate elf_size failed\n");
		return -1;
	}
	printf("[load_enclave] elf size: %d\n", elf_size);
	
    long stack_size = enclave_param->stack_size;
	long untrusted_mem_size = enclave_param->untrusted_mem_size;
	unsigned long untrusted_mem_ptr = enclave_param->untrusted_mem_ptr;
	unsigned long kbuffer_ptr = ENCLAVE_DEFAULT_KBUFFER;
	unsigned int total_pages = total_enclave_page(elf_size, stack_size);
	unsigned long free_mem, elf_entry, meta_offset, meta_blocksize;
	unsigned long order = ilog2(total_pages- 1) + 1;

	total_pages = 0x1 << order;
	if((elf_size > MAX_ELF_SIZE) || (stack_size > MAX_STACK_SIZE) || (untrusted_mem_size > MAX_UNTRUSTED_MEM_SIZE)){
        printf("SIGN_TOOL: eapp memory is out of bound \n");
		return -1;
    }
    printf("[load_enclave] total_pages: %d\n", total_pages);
	
    enclave_mem_t* enclave_mem = malloc(sizeof(enclave_mem_t));
    int size = total_pages * RISCV_PGSIZE;
    char* addr = (char*)malloc(size + RISCV_PGSIZE);
    if(!addr)
	{
		printf("SIGN_TOOL: can not alloc untrusted mem \n");
		return -1;
	}
    vaddr_t page_addr = (vaddr_t)PAGE_UP((unsigned long)addr);
    // memset(addr, 0, size * sizeof(char));
	enclave_mem_int(enclave_mem, page_addr, size, page_addr);
    
    elf_entry = 0;
	if(penglai_enclave_eapp_preprare(enclave_mem, elf_ptr, elf_size,
				&elf_entry, STACK_POINT, stack_size, &meta_offset, &meta_blocksize))
	{
		printf("SIGN_TOOL: penglai_enclave_eapp_preprare is failed\n");
        return -1;
	}
	if(elf_entry == 0)
	{
		printf("SIGN_TOOL: elf_entry reset is failed \n");
	}

    untrusted_mem_size = 0x1 << (ilog2(untrusted_mem_size - 1) + 1);
	if((untrusted_mem_ptr == 0) && (untrusted_mem_size > 0))
	{
		alloc_umem(untrusted_mem_size, &untrusted_mem_ptr, DEFAULT_UNTRUSTED_PTR, enclave_mem);
	}
	alloc_kbuffer(ENCLAVE_DEFAULT_KBUFFER_SIZE, &kbuffer_ptr, ENCLAVE_DEFAULT_KBUFFER, enclave_mem);

    unsigned char enclave_hash[HASH_SIZE];
    hash_enclave(elf_entry, enclave_mem, (void*)enclave_hash, 0, DEFAULT_UNTRUSTED_PTR, untrusted_mem_size, ENCLAVE_DEFAULT_KBUFFER, ENCLAVE_DEFAULT_KBUFFER_SIZE);
    printf("[load_enclave] hash with nonce: \n");
    printHex(enclave_hash, HASH_SIZE);
    
	memcpy(enclave_css->enclave_hash, enclave_hash, HASH_SIZE);
	*meta_offset_arg = meta_offset;

    free(addr);
    return 0;
}

/* load enclave to 
    (1) parse elf file to get the .note.penglaimeta section offset
    (2) load elf to memory and calculate the enclave_hash, which will be saved in enclave_css 
*/
int load_enclave(const char *eappfile, enclave_css_t *enclave_css, unsigned long *meta_offset)
{
    int ret = 0;
    struct elf_args* enclaveFile;
    struct penglai_enclave_user_param* user_param;
    enclaveFile = malloc(sizeof(struct elf_args));
    elf_args_init(enclaveFile, eappfile);
    if(!elf_valid(enclaveFile))
    {
        printf("error when initializing enclaveFile\n");
        ret = -1;
        goto out;
    }
    user_param = malloc(sizeof(struct penglai_enclave_user_param));
    init_enclave_user_param(user_param, enclaveFile);
    ret = penglai_enclave_create(user_param, enclave_css, meta_offset);
    
    free(user_param);
out:
    elf_args_destroy(enclaveFile);
    free(enclaveFile);
    return ret;
}

int update_metadata(const char *eappfile, const enclave_css_t *enclave_css, uint64_t meta_offset)
{
    if(eappfile == NULL || enclave_css == NULL || meta_offset < 0){
		printf("ERROR: invalid params\n");
		return -1;
	};
    return write_data_to_file(eappfile, "rb+", (unsigned char *)enclave_css, sizeof(enclave_css_t), meta_offset);
}

int read_metadata(const char *eappfile, enclave_css_t *enclave_css, uint64_t meta_offset)
{
    if(eappfile == NULL || enclave_css == NULL || meta_offset < 0){
		printf("ERROR: invalid params\n");
		return -1;
	};
    return read_file_to_buf(eappfile, (unsigned char *)enclave_css, sizeof(enclave_css_t), meta_offset);
}

bool dump_enclave_metadata(const char *eappfile, const char *dumpfile)
{
    enclave_css_t enclave_css;
    unsigned long meta_offset;
    int ret = 0;

    ret = load_enclave(eappfile, &enclave_css, &meta_offset);
    if(ret != 0){
        return false;
    }

    memset(&enclave_css, 0, sizeof(enclave_css_t));
    ret = read_metadata(eappfile, &enclave_css, meta_offset);
    if(ret != 0){
        return false;
    }

    ret = write_data_to_file(dumpfile, "wb", (unsigned char *)&enclave_css, sizeof(enclave_css_t), 0);
    if(ret != 0){
        return false;
    }
    return true;
}

static bool cmdline_parse(unsigned int argc, char *argv[], int *mode, const char **path)
{
    assert(mode!=NULL && path != NULL);
    if(argc<2)
    {
        printf("SIGN_TOOL: Lack of parameters.\n");
        return false;
    }
    if(argc == 2 && !strcmp(argv[1], "-help"))
    {
         printf(USAGE_STRING);
         *mode = -1;
         return true;
    }
    
    enum { PAR_REQUIRED, PAR_OPTIONAL, PAR_INVALID };
    typedef struct _param_struct_{
        const char *name;          //options
        char *value;               //keep the path
        int flag;                  //indicate this parameter is required(0), optional(1) or invalid(2)
    }param_struct_t;               //keep the parameter pairs

    param_struct_t params_sign[] = {
        {"-enclave", NULL, PAR_REQUIRED},
        {"-key", NULL, PAR_REQUIRED},
        {"-out", NULL, PAR_REQUIRED},
        {"-sig", NULL, PAR_INVALID},
        {"-unsigned", NULL, PAR_INVALID},
        {"-dumpfile", NULL, PAR_OPTIONAL}};
    param_struct_t params_gendata[] = {
        {"-enclave", NULL, PAR_REQUIRED},
        {"-key", NULL, PAR_INVALID},
        {"-out", NULL, PAR_REQUIRED},
        {"-sig", NULL, PAR_INVALID},
        {"-unsigned", NULL, PAR_INVALID},
        {"-dumpfile", NULL, PAR_INVALID}};
    param_struct_t params_catsig[] = {
        {"-enclave", NULL, PAR_REQUIRED},
        {"-key", NULL, PAR_REQUIRED},
        {"-out", NULL, PAR_REQUIRED},
        {"-sig", NULL, PAR_REQUIRED},
        {"-unsigned", NULL, PAR_REQUIRED},
        {"-dumpfile", NULL, PAR_OPTIONAL}};
    param_struct_t params_dump[] = {
        {"-enclave", NULL, PAR_REQUIRED},
        {"-key", NULL, PAR_INVALID},
        {"-out", NULL, PAR_INVALID},
        {"-sig", NULL, PAR_INVALID},
        {"-unsigned", NULL, PAR_INVALID},
        {"-dumpfile", NULL, PAR_REQUIRED}};

    const char *mode_m[] ={"sign", "gendata","catsig", "dump"};
    param_struct_t *params[] = {params_sign, params_gendata, params_catsig, params_dump};
    
	unsigned int tempidx=0;
    for(; tempidx<sizeof(mode_m)/sizeof(mode_m[0]); tempidx++)
    {
        if(!strcmp(mode_m[tempidx], argv[1]))//match
        {
            break;
        }
    }
    unsigned int tempmode = tempidx;
    if(tempmode>=sizeof(mode_m)/sizeof(mode_m[0]))
    {
        printf("Cannot recognize the command \"%s\".\nCommand \"sign/gendata/catsig\" is required.\n", argv[1]);
        return false;
    }

    unsigned int params_count = (unsigned)(sizeof(params_sign)/sizeof(params_sign[0]));
    for(unsigned int i=2; i<argc; i++)
    {
        unsigned int idx = 0;
        for(; idx<params_count; idx++)
        {
            if(strcmp(argv[i], params[tempmode][idx].name)==0) //match
            {
                if((i<argc-1)&&(strncmp(argv[i+1], "-", 1)))  // assuming pathname doesn't contain "-"
                {
                    if(params[tempmode][idx].value != NULL)
                    {
                        printf("Repeatly specified \"%s\" option.\n", params[tempmode][idx].name);
                        return false;
                    }
                    params[tempmode][idx].value = argv[i+1];
                    i++;
                    break;
                }
                else     //didn't match: 1) no path parameter behind option parameter 2) parameters format error.
                {
                    printf("The File name is not correct for \"%s\" option.\n", params[tempmode][idx].name);
                    return false;
                }
            }
        }
        if(idx == params_count)
        {
            printf("Cannot recognize the option \"%s\".\n", argv[i]);
            return false;
        }
    }

    for(unsigned int i = 0; i < params_count; i++)
    {
        if(params[tempmode][i].flag == PAR_REQUIRED && params[tempmode][i].value == NULL)
        {
            printf("Option \"%s\" is required for the command \"%s\".\n", params[tempmode][i].name, mode_m[tempmode]);
            return false;
        }
        if(params[tempmode][i].flag == PAR_INVALID && params[tempmode][i].value != NULL)
        {
            printf("Option \"%s\" is invalid for the command \"%s\".\n", params[tempmode][i].name, mode_m[tempmode]);
            return false;
        }
    }
    
    for(unsigned int i = 0; i < params_count-1; i++)
    {
        if(params[tempmode][i].value == NULL)
            continue;
        for(unsigned int j=i+1; j < params_count; j++)
        {
            if(params[tempmode][j].value == NULL)
                continue;
            if(strlen(params[tempmode][i].value) == strlen(params[tempmode][j].value) &&
                !strncmp(params[tempmode][i].value, params[tempmode][j].value, strlen(params[tempmode][i].value)))
            {
                printf("Option \"%s\" and option \"%s\" are using the same file path.\n", params[tempmode][i].name, params[tempmode][j].name);
                return false;
            }
        }
    }
    // Set output parameters
    for(unsigned int i = 0; i < params_count; i++)
    {
        path[i] = params[tempmode][i].value;
    }

    *mode = tempmode;
    return true;
}

int main(int argc, char* argv[])
{
    printf("Welcome to PENGLAI sign_tool!\n");

	const char *path[8] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
	int res = -1, mode = -1;
	//Parse command line
    if(cmdline_parse(argc, argv, &mode, path) == false)
    {
        printf(USAGE_STRING);
        goto clear_return;
    }
    if(mode == -1) // User only wants to get the help info
    {
        res = 0;
        goto clear_return;
    }
	else if(mode == DUMP)
    {
        // dump metadata info
        if(dump_enclave_metadata(path[ELF], path[DUMPFILE]) == false)
        {
            printf("Failed to dump metadata info to file \"%s\".\n.", path[DUMPFILE]);
            goto clear_return;
        }
        printf("Succeed.\n");
        res = 0;
        goto clear_return;
    }
    else if(mode == SIGN)
	{
        printf("SIGN enclave: %s, keyfile: %s, output: %s, dumpfile(optional): %s\n", 
            path[ELF], path[KEY], path[OUTPUT], (path[DUMPFILE] ? path[DUMPFILE] : "--"));
        // load elf
        enclave_css_t enclave_css;
        unsigned long meta_offset;
        if(load_enclave(path[ELF], &enclave_css, &meta_offset) < 0){
            printf("ERROR: load enclave failed!\n");
            goto clear_return;
        }

        // parse private key, sign and verify
        unsigned char *private_key = (unsigned char *)malloc(PRIVATE_KEY_SIZE);
        parse_priv_key_file(path[KEY], private_key, enclave_css.user_pub_key);
        sign_enclave((struct signature_t *)(enclave_css.signature), enclave_css.enclave_hash, HASH_SIZE, private_key);

        printf("[sign_enclave] signature:\n");
        printHex(enclave_css.signature, SIGNATURE_SIZE);
        // generate_signature_DER("sig-der", enclave_css.signature);
        printf("[sign_enclave] enclave hash:\n");
        printHex(enclave_css.enclave_hash, HASH_SIZE);
        printf("[sign_enclave] private_key: \n");
        printHex(private_key, PRIVATE_KEY_SIZE);
        printf("[sign_enclave] public_key: \n");
        printHex(enclave_css.user_pub_key, PUBLIC_KEY_SIZE);
        printf("begin verify\n");
        int ret = verify_enclave((struct signature_t *)(enclave_css.signature), enclave_css.enclave_hash, HASH_SIZE, enclave_css.user_pub_key);
        if(ret != 0){
            printf("ERROR: verify enclave_css struct failed!\n");
            goto clear_return;
        } else {
            printf("verify enclave's signature successfully.\n");
        }

        // generate out
        copy_file(path[ELF], path[OUTPUT]);
        update_metadata(path[OUTPUT], &enclave_css, meta_offset);

        //dump
        if(path[DUMPFILE] != NULL && dump_enclave_metadata(path[OUTPUT], path[DUMPFILE]) == false)
        {
            printf("Failed to dump metadata info to file \"%s\".\n.", path[DUMPFILE]);
            goto clear_return;
        }
	}
    else if(mode == GENDATA)
    {
        printf("GENDATA enclave: %s, output: %s, \n", path[ELF], path[OUTPUT]);
        // load elf
        enclave_css_t enclave_css;
        unsigned long meta_offset;
        if(load_enclave(path[ELF], &enclave_css, &meta_offset) < 0){
            printf("ERROR: load enclave failed!\n");
        }
        // output enclave hash
        write_data_to_file(path[OUTPUT], "wb", enclave_css.enclave_hash, HASH_SIZE, 0);
    }
    else if(mode == CATSIG)
    {
        printf("CATSIG enclave: %s, keyfile: %s, output: %s, signatrue: %s, unsigned hash: %s, dumpfile(optional): %s\n", 
            path[ELF], path[KEY], path[OUTPUT], path[SIG], path[UNSIGNED], (path[DUMPFILE] ? path[DUMPFILE] : "--"));
        // load enclave to get meta_offset
        enclave_css_t enclave_css;
        unsigned long meta_offset;
        if(load_enclave(path[ELF], &enclave_css, &meta_offset) < 0){
            printf("ERROR: load enclave failed!\n");
            goto clear_return;
        }
        // parse public key, verify signature
        unsigned char *hash = (unsigned char *)malloc(HASH_SIZE);
        read_file_to_buf(path[UNSIGNED], hash, HASH_SIZE, 0);
        printf("hash:\n");
        printHex(hash, HASH_SIZE);
        unsigned char *public_key = (unsigned char *)malloc(PUBLIC_KEY_SIZE);
        parse_pub_key_file(path[KEY], public_key);
        printf("public key:\n");
        printHex(public_key, PUBLIC_KEY_SIZE);
        printf("publickey finish\n");
        unsigned char *signature = (unsigned char *)malloc(SIGNATURE_SIZE);
        parse_signature_DER(path[SIG], signature);
        printf("signature:\n");
        printHex(signature, SIGNATURE_SIZE);
        int ret = verify_enclave((struct signature_t *)signature, hash, HASH_SIZE, public_key);
        if(ret != 0){
            printf("ERROR: verify signature failed!\n");
            goto clear_return;
        }
        // append signature to eappfile
        copy_file(path[ELF], path[OUTPUT]);
        if(memcmp(enclave_css.enclave_hash, hash, HASH_SIZE) != 0){
            printf("ERROR: UNSIGNED hash is wrong.\n");
            goto clear_return;
        }
        memcpy(enclave_css.enclave_hash, hash, HASH_SIZE);
        memcpy(enclave_css.signature, signature, SIGNATURE_SIZE);
        memcpy(enclave_css.user_pub_key, public_key, PUBLIC_KEY_SIZE);
        update_metadata(path[OUTPUT], &enclave_css, meta_offset);
        //dump
        if(path[DUMPFILE] != NULL && dump_enclave_metadata(path[OUTPUT], path[DUMPFILE]) == false)
        {
            printf("Failed to dump metadata info to file \"%s\".\n.", path[DUMPFILE]);
            goto clear_return;
        }
    }
    printf("Succeed.\n");

clear_return:
    return 0;
}
