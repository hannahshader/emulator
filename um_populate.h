/******************************************************************************
*       um_populate.h
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the function declarations for functions needed to
*       populate the zero segment with instructions, including looping through
*       the file, extracting characters, packing them into a 32 bit word, and
*       adding them to the zero segment. 
*   
******************************************************************************/

#ifndef UM_POPULATE_
#define UM_POPULATE_

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "memory_type.h"
#include "bitpack.h"

void populate_instructions(FILE *input, memory mem);
bool make_word(FILE *input, memory mem);

#endif 