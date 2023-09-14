/******************************************************************************
*       um_operations.h
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the declarations for the different functions needed
*       to execute the user's program instructions, including a function to
*       loop through the instructions, parse the instructions, and perform the
*       14 um operations including add, multiply, load value, map segment, etc.
*   
******************************************************************************/

#ifndef UM_OPERATIONS_
#define UM_OPERATIONS_

#include <stdio.h>
#include <stdbool.h>
#include "assert.h"
#include "seq.h"
#include "memory_type.h"
#include "bitpack.h"

typedef struct operation_info *operation_info;

typedef uint32_t Um_instruction;

void execute_program(memory mem, uint32_t *r);
uint32_t get_code(Um_instruction instruction);
void get_values(Um_instruction instruction, operation_info info);
void conditional_move(operation_info info);
void segmented_load(operation_info info);
void segmented_store(operation_info info);
void addition(operation_info info);
void multiplication(operation_info info);
void division(operation_info info);
void bitwise_nand(operation_info info);
void map(operation_info info);
void unmap(operation_info info);
void output(operation_info info);
void input(operation_info info);
uint32_t load_program(operation_info info);
void load_value(operation_info info);
void check_values(operation_info info);

#endif 