/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserve.
**************************************************************************/

#include "Random.h"
#include "miracl.h"
#include <string.h>
#include <time.h>

/*
 * Generate a random number with 'size' bytes length. It can be 
 * used to generate Key and IV in symmetric encryption and so on.
 * 
 * When it's first called, random number seed will be set as time(0)
 */
void penglai_read_rand(unsigned char rand[], int size)
{
	big x;
    static int is_first_call = 1;
    int blocks, fill_size;

	char mem[MR_BIG_RESERVE(1)];
	memset(mem, 0, MR_BIG_RESERVE(1));
	x = mirvar_mem(mem, 0);

    if(is_first_call){
        mirsys(128, 16);
        irand((unsigned int)time(NULL));
        is_first_call = 0;
    }
    
    blocks = (size + 32 - 1) / 32;
    for(int i = 0; i < blocks; i++){
        fill_size = (i == (blocks-1)) ? (size - (blocks-1) * 32) : 32;
        /* generate random number '2 * fill_size' digits long to base 16 */
        bigdig(2 * fill_size, 16, x);
        big_to_bytes(fill_size, x, (char *)(rand + i * 32), TRUE);
    }
}
