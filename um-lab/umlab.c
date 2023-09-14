/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) {
        Um_instruction result = 0;
        result = Bitpack_newu(result, 4, 28, op);
        result = Bitpack_newu(result, 3, 0, rc);
        result = Bitpack_newu(result, 3, 3, rb);
        result = Bitpack_newu(result, 3, 6, ra);

        return result;
}

Um_instruction loadval(unsigned ra, unsigned val) {
        Um_instruction result = 0;
        result = Bitpack_newu(result, 4, 28, 13);
        result = Bitpack_newu(result, 3, 25, ra);
        result = Bitpack_newu(result, 25, 0, val);

        return result;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction output(Um_register c) {
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction map(Um_register b, Um_register c) {
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_register c) {
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c) {
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c) {
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction input(Um_register c) {
        return three_register(IN, 0, 0, c);
}



/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_load_test(Seq_T stream){
        append(stream, loadval(r1, 'G'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'o'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'o'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_addition_test(Seq_T stream) {
        append(stream, three_register(ADD, r1, r2, r3));
        append(stream, halt());
} 

void build_print_unit_test(Seq_T stream) {
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, three_register(ADD, r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
} 

void build_multiplication_test(Seq_T stream){
        append(stream, loadval(r1, 5));
        append(stream, loadval(r2, 20));
        append(stream, three_register(MUL, r3, r1, r2));
        append(stream, output(r3)); 
        append(stream, halt());
}

void build_division_test(Seq_T stream){
        append(stream, loadval(r1, 2400));
        append(stream, loadval(r2, 20));
        append(stream, three_register(DIV, r3, r1, r2));
        append(stream, output(r3)); 
        append(stream, halt());
}

void build_math_mod_test(Seq_T stream){
        append(stream, loadval(r1, 5000));
        append(stream, loadval(r2, 858993));
        append(stream, three_register(MUL, r3, r1, r2));
        append(stream, loadval(r1, 2413));
        append(stream, three_register(ADD, r2, r1, r3));
        append(stream, output(r2));
        append(stream, loadval(r1, 322445));
        append(stream, loadval(r2, 13320));
        append(stream, three_register(MUL, r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_reuse_id_unmap_map_test(Seq_T stream){

        // map segment (size 3)
        append(stream, loadval(r1, 3));
        append(stream, map(r2, r1));

        // store values in segment
        append(stream, loadval(r3, 65));
        append(stream, loadval(r4, 0));
        append(stream, sstore(r2, r4, r3));

        append(stream, loadval(r3, 66));
        append(stream, loadval(r5, 1));
        append(stream, sstore(r2, r5, r3));

        append(stream, loadval(r3, 67));
        append(stream, loadval(r6, 2));
        append(stream, sstore(r2, r6, r3));

        // load segment and output values
        append(stream, sload(r1, r2, r4));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r5));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r6));
        append(stream, output(r1));

        // unmap segment
        append(stream, unmap(r2));

        // map another segment
        append(stream, loadval(r1, 3));
        append(stream, map(r7, r1));

        // store diff values in segment
        append(stream, loadval(r3, 68));
        append(stream, loadval(r4, 0));
        append(stream, sstore(r2, r4, r3));

        append(stream, loadval(r3, 69));
        append(stream, loadval(r5, 1));
        append(stream, sstore(r2, r5, r3));

        append(stream, loadval(r3, 70));
        append(stream, loadval(r6, 2));
        append(stream, sstore(r2, r6, r3));

        // load segments using the unmapped segment id
        append(stream, sload(r1, r2, r4));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r5));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r6));
        append(stream, output(r1));

        append(stream, halt());
}

void build_mapping_test(Seq_T stream){
         // map segment (size 3)
        append(stream, loadval(r1, 3));
        append(stream, map(r2, r1));

        // store values in segment
        append(stream, loadval(r3, 65));
        append(stream, loadval(r4, 0));
        append(stream, sstore(r2, r4, r3));

        append(stream, loadval(r3, 66));
        append(stream, loadval(r5, 1));
        append(stream, sstore(r2, r5, r3));

        append(stream, loadval(r3, 67));
        append(stream, loadval(r6, 2));
        append(stream, sstore(r2, r6, r3));

        // load segment and output values
        append(stream, sload(r1, r2, r4));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r5));
        append(stream, output(r1));
        append(stream, sload(r1, r2, r6));
        append(stream, output(r1));
        append(stream, halt());
}

void build_input_test(Seq_T stream){
        append(stream, input(r1));
        append(stream, input(r2));
        append(stream, input(r3));
        append(stream, input(r4));
        append(stream, input(r5));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, output(r3));
        append(stream, output(r4));
        append(stream, output(r5));
        append(stream, halt());
}

void build_conditional_move_test(Seq_T stream){
        append(stream, loadval(r4, 65));
        append(stream, loadval(r1, 65));
        append(stream, loadval(r2, 80));
        append(stream, three_register(DIV, r3, r1, r2));
        append(stream, three_register(CMOV, r4, r2, r3));
        append(stream, output(r4));
        append(stream, halt());
}

void build_nand_test(Seq_T stream){
        append(stream, loadval(r4, 89));
        append(stream, loadval(r1, 78));
        append(stream, loadval(r2, 78));
        append(stream, three_register(NAND, r3, r1, r2));
        append(stream, three_register(CMOV, r4, r1, r3));
        append(stream, output(r4));
        append(stream, halt());
}

void build_load_program_test(Seq_T stream){
        append(stream, loadval(r1, 65));
        append(stream, loadval(r2, 66));
        append(stream, loadval(r3, 0));
        append(stream, loadval(r4, 6));
        append(stream, three_register(LOADP, 0, r3, r4));
        append(stream, output(r1));
        append(stream, output(r2));
        append(stream, halt());
}