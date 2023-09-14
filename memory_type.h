/******************************************************************************
*       memory_type.h
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the declarations for the memory data type's 
*       functions. The memory type emulates a memory system, storing segements
*       of memory, which hold sequences of uint32_t words. The memory functions
*       create, free and manipulate the memory. This includes creating the
*       memory type, creating new segments, adding words to memory, getting
*       words from memory, freeing segments and memory, etc. 
*   
******************************************************************************/

#ifndef MEMORY_TYPE_
#define MEMORY_TYPE_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "assert.h"
#include "seq.h"


typedef struct memory *memory;

memory new_memory();
uint32_t get_memory(memory mem, uint32_t seg, int word);
void set_memory(memory mem, uint32_t seg, uint32_t word);
uint32_t new_seg(memory mem, int length);
void free_segment(memory mem, uint32_t id);
void free_memory(memory mem);
void duplicate_instructions(memory mem, uint32_t seg);
void set_word(memory mem, uint32_t seg, uint32_t index, uint32_t word);

#endif 