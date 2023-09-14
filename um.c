/******************************************************************************
*       um.c
*       By: Kalyn (kmuhle01) and Hannah (hshade01)
*       4/1/2023
*
*       Comp40 Project 6: um
*
*       This file contains the main function of the um program. It creates the
*       memory and registers, as well as opens the user's .um file. It also
*       uses um_populate to get the user's instructions, and um_operations to
*       execute the program. 
*   
******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "um_populate.h"
#include "um_operations.h"
#include "assert.h"
#include "memory_type.h"
#include <sys/stat.h>

/*
*       Description: Initializes memory and registers to read in from a file
*       and run the program. Sets and frees memory.  
*
*       In/Out Expectations: Expects 2 command line arguments, with the second 
*       being a valid file name. Asserts that file size is appropriate, or 
*       returns exit failure if file can't be opened/wasn't supplied. Otherwise
*       returns exit success.
*/
int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Error: Incorrect number of arguments.\n");
                return EXIT_FAILURE;
        }
        
        /* checks that program won't run if there's an incorrect file size */
        char *filename = argv[1];
        struct stat st;
        stat(filename, &st);
        int size = st.st_size;
        assert(size % 4 == 0);
        
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
                fprintf(stderr, "Error: file can't be opened.\n");
                return EXIT_FAILURE;
        }
        
        memory mem = new_memory();
        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        populate_instructions(fp, mem);
        execute_program(mem, registers);
        free_memory(mem);
        
        fclose(fp);

        return EXIT_SUCCESS;
}




