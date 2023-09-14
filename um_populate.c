/******************************************************************************
*       um_populate.c
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains functions to read in characters from the user's 
*       .um file and bitpack them into 32 bit words representing um
*       instructions. The instructions are stored in the 0 segment in memory
*   
******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "um_populate.h"
#include "memory_type.h"
#include "bitpack.h"
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef uint32_t Um_instruction;

/*
*       Description: Calls function to create words from the characters 
*       in a um file until the end of file has been reached.
*
*       In/Out Expectations: Expects a valid input file and memory type 
*       (linked from our module memory_type) from main. Returns void.
*/
void populate_instructions(FILE *input, memory mem)
{
        assert(input != NULL);

        new_seg(mem, 0); 

        bool is_eof;
        do {
                is_eof = make_word(input, mem);
        } while (!is_eof);
}

/*
*       Description: Populates words from the characters read in, and adds 
*       them to the first segment in memory. 
*
*       In/Out Expectations: Expects a valid input file and memory type 
*       (linked from our module memory_type) from the populate 
*       instruction function. Uses a linked module bitpack to create a word 
*       from 4 characters read in. Returns true when the end of file is 
*       reached, otherwise returns false. 
*/
bool make_word(FILE *input, memory mem)
{
        int c;
        Um_instruction word = 0;
        for (int i = 24; i >= 0; i = i - 8) {
                c = getc(input);
                if (c == EOF) {
                        return true;
                }
                word = Bitpack_newu(word, 8, i, c);
        }
        set_memory(mem, 0, word);
        return false;
}