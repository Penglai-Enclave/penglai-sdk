/************************************************************************
  Copyright (c) IPADS@SJTU 2021. All rights reserve.
**************************************************************************/

#pragma once

/*
 * Generate a random number with 'size' bytes length. It can be 
 * used to generate Key and IV in symmetric encryption and so on.
 * 
 * When it's first called, random number seed will be set as time(0)
 */
void penglai_read_rand(unsigned char rand[], int size);
