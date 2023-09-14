/******************************************************************************
*       memory_type.c
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the implementations for the memory type, which is a 
*       data type that emulates a memory system. The memory struct 
*       contains a memory_seq, which is a Seq_T that holds mapped memory 
*       segments. The memory segments are also represented as Seq_Ts, which 
*       then hold uint32_t words. The memory struct also contains an 
*       unmapped_ids Seq_T, which holds the segment ids of all of the unmapped
*       segments.
*   
******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "seq.h"
#include "memory_type.h"
#include "assert.h"
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

struct memory {
        Seq_T memory_seq; 
        Seq_T unmapped_ids; 
};


/*
*       Description: A function that creates a new struct of type memory,
*       which holds two sequences. One sequence represents our memory, with
*       each element (segmeent) of the sequence holding a sequence of words. 
*       The other sequence holds the id numbers, or index numbers in the
*       first sequence that have been unmapped with the unmap operation.
*       In this function, both sequences are set as empty.
*
*       In/Out Expectations: Expects nothing, only that the struct of memory
*       has been defined. Returns a memory struct type populated with
*       empty sequences. 
*       Memory expected to be freed by user (using memory_free)
*/
memory new_memory() 
{
        Seq_T sequence = Seq_new(0);
        Seq_T ids = Seq_new(0);

        memory new = malloc(sizeof(*new));
        assert(new != NULL);

        new->memory_seq = sequence;
        new->unmapped_ids = ids;

        return new;
}

/*
*       Description: A function that adds an id to the sequence in the memory
*       struct holding the ids of sequence segments that have been unmapped. 
*
*       In/Out Expectations: Expects a valid memory type, and a uint32_t
*       representning the index of the segment in memory that has been 
*       uunmapped.
*       Returns nothing, adds the passed id to the sequence of unmapped ids. 
*/
void add_to_unmapped_seq(memory curr, uint32_t id) 
{
        Seq_addhi(curr->unmapped_ids, (void *)(uintptr_t)id);
}

/*
*       Description: A function that adds a sequence of words (memory segment)
*       to the memory at the next avaliable slot. This is either at the end of
*       the memory, or at the first slot previously unmapped.
*
*       In/Out Expectations: Expects a valid memory type, and an int reprenting
*       how many words will be in memory segment that will be added. Returns
*       the index in memory that the new segment has been added to. 
*/
uint32_t new_seg(memory mem, int length) 
{
        Seq_T segment = Seq_new(length);
        for (int i = 0; i < length; i++){
                Seq_addhi(segment, (void *)(uintptr_t)0);
        }

        uint32_t id;

        /**/
        if (Seq_length(mem->unmapped_ids) == 0){
                id = Seq_length(mem->memory_seq);
                Seq_addhi(mem->memory_seq, segment);
        } else {
                id = (uint32_t)(uintptr_t)Seq_remlo(mem->unmapped_ids);
                Seq_put(mem->memory_seq, id, segment);
        }
        
        return id;
}

/*
*       Description: A function that gets a word in memory given the segment
*       and index within the segment that it's located at. 
*
*       In/Out Expectations: Expects a valid memory type, a uint32_t 
*       representing the segment to get memory at, and a int that represents 
*       the index in the segment of the desired word. 
*       Returns a uint32_t of the word at the 
*       specified index of the specifed segment. Note: behavior
*       undefined when the memory struct doesn't have a word mapped in the 
*       given segment at the given index.
*       TO DO: CHECK THAT THE LAST LINE IS TRUE. 
*/
uint32_t get_memory(memory mem, uint32_t seg, int index) 
{
        Seq_T segment_to_get = Seq_get(mem->memory_seq, seg);
        return (uint32_t)(uintptr_t)Seq_get(segment_to_get, index);
}

/*
*       Description: A function that sets a word in memory given the segment
*       and adds it to the end of the segment. 
*
*       In/Out Expectations: Expects a valid memory type, a uint32_t 
*       representing the segment to add the word, and a uint32_t of the 
*       new word to add.
*       Returns nothing.  Note: behavior undefined when the specified segment 
*       isn't mapped.
*/
void set_memory(memory mem, uint32_t seg, uint32_t word) 
{
        Seq_T segment_to_add = Seq_get(mem->memory_seq, seg);
	Seq_addhi(segment_to_add, (void *)(uintptr_t)word);
}

/*
*       Description: A function that sets a word in memory given the segment
*       and index within the segment that it should be located at. 
*
*       In/Out Expectations: Expects a valid memory type, a uint32_t 
*       representing the segment to add the word, and a int that represents
*       the index in the in the segment to add the new word, and a uint32_t 
*       of the new word to add. Returns nothing.  
*       Note: behavior undefined when the specified segment 
*       isn't mapped.
*/
void set_word(memory mem, uint32_t seg, uint32_t index, uint32_t word)
{
        Seq_T segment_to_add = Seq_get(mem->memory_seq, seg);
        Seq_put(segment_to_add, index, (void *)(uintptr_t)word);
}

/*
*       Description: A function that copies a segment in memory, and replaces
*       the first segment in memory, storing the instructions, with that 
*       memory segment copy. 
*       
*       In/Out Expectations: Expects a valid memory type, and a uint32_t 
*       representing the index of the segment to copy. Deallocates and deletes
*       the memory segment currently in the first index of memory. 
*       Allocates memory for the copy of the segment and adds it to the 
*       first memory index. Returns nothing.  
*       Note: behavior undefined when the specified segment isn't mapped.
*/
void duplicate_instructions(memory mem, uint32_t seg)
{
        free_segment(mem, 0);

        Seq_T old_seq = Seq_get(mem->memory_seq, seg);
        Seq_T new_seq = Seq_new(Seq_length(old_seq));

        for (int i = 0; i < Seq_length(old_seq); i++) {
                Seq_addhi(new_seq, Seq_get(old_seq, i));
        }
        Seq_put(mem->memory_seq, 0, new_seq);
}


/*
*       Description: A function that frees memory associated with one segment
*       in memory, and adds the index of the segment that was freed to our
*       sequence of unmapped segments.
*       
*       In/Out Expectations: Expects a valid memory type, and a uint32_t 
*       representing the index of the segment to free. Deallocates and deletes
*       the memory segment if the memory segment has been mapped. Returns
*       nothing.
*/
void free_segment(memory mem, uint32_t id) 
{
        if(id != 0){
                add_to_unmapped_seq(mem, id);
        }
        Seq_T segment = Seq_get(mem->memory_seq, id);
        if (segment != NULL) {
                Seq_free(&segment);
        }
        Seq_put(mem->memory_seq, id, NULL);
        assert(segment == NULL);
}

/*
*       Description: A function that frees all segments in a memory struct.
*       
*       In/Out Expectations: Expects a valid memory type. Calls the
*       free_segment function to free all words in each segment. Returns
*       nothing.
*/
void free_memory(memory mem) 
{
	for (int i = 0; i < Seq_length(mem->memory_seq); i++) {
                free_segment(mem, i);
	}
	Seq_free(&(mem->memory_seq));
        Seq_free(&(mem->unmapped_ids));
        free(mem);
}

