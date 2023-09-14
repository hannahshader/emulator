
/******************************************************************************
*       um_operations.c
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the functions needed to execute the user's program. 
*       It loops through each instruction, parses the instruction, and executes
*       the appropriate um operation. 
*   
******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "assert.h"
#include "seq.h"
#include "memory_type.h"
#include "um_operations.h"
#include "bitpack.h"
#include <inttypes.h>

#define MAX_VAL 255
#define MIN_VAL 0
#define MOD_VAL 4294967296 /* equals 2^32 because using uint32_t */

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LOADV
} Um_opcode;

/*
*       Description: Stores the memory and registers, and
*       all possible values that may be used for an operation
*       regardless of whether it's a three register operation
*       or otherwise. This struct allows us to pass all 
*       needed information to functions that perform operations
*       that access/modify the reigsters and the memory.
*/
struct operation_info {
        uint32_t *registers;
        memory mem;
        uint32_t ra;
        uint32_t rb;
        uint32_t rc;
        uint32_t ra_load;
        uint32_t load_value;
};


/*
*       Description: Gets instructions from memory, and iterates 
*       through/performs all instructions.
*
*       In/Out Expectations: Expects a valid memory type, and a pointer
*       to the array of registers. Expects that the first segment in memory
*       is populated with the instructions from the file. Uses type from 
*       module memory_type. Returns nothing.
*/
void execute_program(memory mem, uint32_t *r)
{
        uint32_t program_counter = 0;
        Um_opcode opcode; 
        operation_info curr_info = malloc(sizeof(*curr_info));
        curr_info->registers = r;
        curr_info->mem = mem;

        do {
                uint32_t curr_instruction = 
                get_memory(mem, 0, program_counter);
                get_values(curr_instruction, curr_info);
                opcode = get_code(curr_instruction);
                switch(opcode){
                case CMOV:
                        conditional_move(curr_info);
                        break;
                
                case SLOAD:
                        segmented_load(curr_info);
                        break;

                case SSTORE:
                        segmented_store(curr_info);
                        break;

                case ADD:
                        addition(curr_info);
                        break;

                case MUL:
                        multiplication(curr_info);
                        break;

                case DIV:
                        division(curr_info);
                        break;

                case NAND:
                        bitwise_nand(curr_info);
                        break;

                case HALT:
                        break;
                
                case MAP:
                        map(curr_info);
                        break;

                case UNMAP:
                        unmap(curr_info);
                        break;

                case OUT:
                        output(curr_info);
                        break;

                case IN:
                        input(curr_info);
                        break;

                case LOADP:
                        program_counter = load_program(curr_info); 
                        /* decrements so that first program instruction runs */
                        program_counter--;
                        break;

                case LOADV:
                        load_value(curr_info);
                        break;
                }
                program_counter++;
                check_values(curr_info);
        } while (opcode != HALT);

        free(curr_info);
}

/*
*       Description: Gets instruction from word representing an instruction.
*
*       In/Out Expectations: Expects a word with an instruction encoded in the
*       first four bits. Returns a uint32_t with the 4 bit code corresponding
*       to the instruction.
*/
uint32_t get_code(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 4, 28);
}

/*
*       Description: Gets the values involved in the operation from a word 
*       representing the instruction. Collects all instructions regardless 
*       of whether the instruction is in other instruction or three's 
*       register format.
*
*       In/Out Expectations: Expects a word with an instruction encoded bits
*       specified by either the three register or other instruction form. 
*       Returns a Um_instruction struct with all fields populated with the 
*       potential values of each register. Returns nothing.
*/
void get_values(Um_instruction instruction, operation_info info) 
{
        info->ra = Bitpack_getu(instruction, 3, 6);
        info->rb = Bitpack_getu(instruction, 3, 3);
        info->rc = Bitpack_getu(instruction, 3, 0);
        info->ra_load = Bitpack_getu(instruction, 3, 25);
        info->load_value = Bitpack_getu(instruction, 25, 0);
}

/*
*       Description: Asserts that all register values are less than 7.
*
*       In/Out Expectations: Expects a operation_info struct. Returns nothing.
*       Fails program if one register value is not less than 7.
*/
void check_values(operation_info info) 
{
        assert(info->ra <= 7);
        assert(info->rb <= 7);
        assert(info->rc <= 7);
        assert(info->ra_load <= 7);
}

/*
*       Description: A function that moves the value in register b to 
*       register c.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from a conditional move instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void conditional_move(operation_info info)
{
        if(info->registers[info->rc] != 0){
                info->registers[info->ra] = info->registers[info->rb];
        }
}

/*
*       Description: A function that moves the value from memory in the
*       segment specified by register and the word in the segment 
*       specified by register c to register a. 
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from a segmented load instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void segmented_load(operation_info info)
{
        info->registers[info->ra] = 
        get_memory(info->mem, info->registers[info->rb], 
        info->registers[info->rc]);
}

/*
*       Description: A function that sets a word in memory with the
*       value in register c. The segment in memory where the new word
*       is added is specified by register a, and the index in this 
*       segment is specified by register b.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from a segmented store instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void segmented_store(operation_info info)
{
        set_word(info->mem, info->registers[info->ra], 
        info->registers[info->rb], info->registers[info->rc]);
}

/*
*       Description: A function adds the value in register b and register
*       c, mods it so it will fit as a uint32_t without wrapping, and 
*       adds it to register a.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from add instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void addition(operation_info info)
{
        uint32_t result = (info->registers[info->rb] + 
        info->registers[info->rc]) % MOD_VAL;
        info->registers[info->ra] = result;
}

/*
*       Description: A function that multiplies the value in register b and 
*       register c, mods it so it will fit as a uint32_t without wrapping,
*       and adds it to register a.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from multiply instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void multiplication(operation_info info)
{
        uint32_t result = 
        (info->registers[info->rb] * info->registers[info->rc]) % MOD_VAL;
        info->registers[info->ra] = result;
}

/*
*       Description: A function that divides the value in register b and 
*       register c, mods it so it will fit as a uint32_t without wrapping,
*       and adds it to register a.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from divides instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing. Undefined behavior if division by 0.
*/
void division(operation_info info)
{
        uint32_t result = 
        (info->registers[info->rb] / info->registers[info->rc]) % MOD_VAL;
        info->registers[info->ra] = result;
}

/*
*       Description: A function that performs the bitwise nand operation
*       with the bit sequences in register b and register c, and adds
*       the result to register a.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from nand instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void bitwise_nand(operation_info info)
{
        uint32_t result = 
        ~(info->registers[info->rb] & info->registers[info->rc]);
        info->registers[info->ra] = result;
}

/*
*       Description: A function that creates a new segment with a number
*       of unintialized words corresponding to the value in register c,
*       and maps it to the index of the memory sequence specified by 
*       register b.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from map instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void map(operation_info info)
{
        info->registers[info->rb] = 
        new_seg(info->mem, info->registers[info->rc]);
}

/*
*       Description: A function that frees the segment in memory
*       with an index in the memory sequence corresponding to the 
*       value in register c.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from unmap instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing. Invalid specified memory indexes are handled
*       in free_segment function.
*/
void unmap(operation_info info)
{
        free_segment(info->mem, info->registers[info->rc]);
}

/*
*       Description: A function that prints to standard output the value
*       in registers specified by register c. 
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from ouput instruction, and
*       includes the array of registers. Asserts that the output value 
*       is between 0 and 255 so the output may be printed as a character.
*       Updates registers array and returns nothing.
*/
void output(operation_info info)
{
        assert(info->registers[info->rc] <= MAX_VAL);
        printf("%c", info->registers[info->rc]);
}

/*
*       Description: A function that reads from a character from 
8       standard input and places its corresponding int in register c.
*       If the character from standard input is the EOF character,
*       places a uint32_t of the bit sequence of all 1s in register c.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from input instruction, and
*       includes the array of registers. Expects a valid input character,
*       which is either the EOF character, or an ASCII value between 0
*       and 255. Asserts this expectation.
*       Updates registers array and returns nothing.
*/
void input(operation_info info)
{
        int input = getchar();
        if(input == EOF) {
                info->registers[info->rc] = ~0;
        }
        else {
                assert(input <= MAX_VAL);
                assert(input >= MIN_VAL);
                info->registers[info->rc] = (uint32_t)input;
        }
}

/*
*       Description: A function that adds a value to register a specified
*       by the load_value field populated in the operation_info struct.
*
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from load value instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing.
*/
void load_value(operation_info info)
{
        info->registers[info->ra_load] = info->load_value;
}

/*
*       Description: A function that gets a new set of instructions from
*       a sequence in memory specified by the an index found in register b,
*       copies it, and replaces the first segment in memory with it.
*       
*       In/Out Expectations: Expects a populated operation_info struct 
*       corresponding to info from load program instruction, and
*       includes the array of registers. Updates registers array and 
*       returns nothing. Note: if the memory segment to replace the 
*       instructions is the first memory segment, this function does
*       nothing.
*/
uint32_t load_program(operation_info info)
{
        if(info->registers[info->rb] != 0) {
                duplicate_instructions(info->mem, info->registers[info->rb]);
        }
        return info->registers[info->rc];
}
