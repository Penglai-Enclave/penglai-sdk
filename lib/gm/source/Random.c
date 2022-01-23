/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserve.

  This file contains the pseudo-random generator provided by PENGLAI sdk.
  It is used to generate 'seed' for SM2, and it can be used to generate
  'key' and 'iv' for SM4.

  Function List:
    1.penglai_set_rand_seed             // set random seed
    2.penglai_read_rand                 // generate a random
**************************************************************************/

#include "Random.h"
#include "miracl.h"
#include <string.h>

static BOOL set_seed = FALSE;
/*
 * FIX ME: A weird bug, if I(Qingyu Shang) remove below two variable's
 * declaration, which are not used here, enclave will halt in mirsys().
 * After locating the wrong code and doing objdump on elf file, I still
 * have no idea with why these two global variable can help with it.
 */
char g_mem[MR_BIG_RESERVE(6)];
char g_mem_point[MR_ECP_RESERVE(2)];

/*
 * User must set a real random as random number seed, otherwise
 * function penglai_read_rand() just output the same result each
 * time the random system initialized.
 * 
 * In order to make 'gm' library available in both Host and Enclave,
 * we let the random number seed be independent of sbi_ecall or time().
 */
void penglai_set_rand_seed(unsigned int seed)
{
	mirsys(128, 16);
    irand(seed);
    set_seed = TRUE;
}

/*
 * Generate a random number with 'size' bytes length. It can be 
 * used to generate Key and IV in symmetric encryption and so on.
 * If random seed haven't been set, it will be set as 0.
 */
void penglai_read_rand(unsigned char rand[], int size)
{
	big x;
    int blocks, fill_size;

	char mem[MR_BIG_RESERVE(1)];
	memset(mem, 0, MR_BIG_RESERVE(1));
	x = mirvar_mem(mem, 0);

    if(!set_seed)
        penglai_set_rand_seed(0);

    blocks = (size + 32 - 1) / 32;
    for(int i = 0; i < blocks; i++){
        fill_size = (i == (blocks-1)) ? (size - (blocks-1) * 32) : 32;
        /* generate random number '2 * fill_size' digits long to base 16 */
        bigdig(2 * fill_size, 16, x);
        big_to_bytes(fill_size, x, (char *)(rand + i * 32), TRUE);
    }
}
