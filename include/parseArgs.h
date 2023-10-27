#pragma once

#include <stdbool.h>
#define NUM_FLAGS 6

/* used as a way of indexing the flags array inside argument_struct_t */
enum flagTypes
{
    help        = 0,
    copy        = 1,
    verify      = 2,
    printDir    = 3,
    extractFile = 4,
    deleteFile  = 5
};

typedef struct
{
    char *inFile;  // input file
    char *outFile; // output file
    char *delFile; // file to delete 
    char *extractFile; // file to extract
    bool flags[NUM_FLAGS];
} argument_struct_t;

/* Extracts the cmd line arguments given to the program and stores the data in
 * an argument_struct_t */
argument_struct_t parseArgs(int argc, char *argv[]);
