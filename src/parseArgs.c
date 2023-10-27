#include <stdlib.h>
#include <getopt.h>

#include "parseArgs.h"

/* Extracts the cmd line arguments given to the program and stores the data in
 * an argument_struct_t */
argument_struct_t parseArgs(int argc, char *argv[])
{
    argument_struct_t argStruct = {}; // Initializes everything to zero/NULL

    /* resetting optind, which is the next element to be processed in argv 
     * back to 1 allow multiple calls of getopt without skipping any values */
    optind = 1;
    int opt = 0;
    while ((opt = getopt(argc, argv, "i:o:x:D:hcvd")) != -1)
    {
        switch (opt)
        {
            case 'i':
                argStruct.inFile = optarg;
                break;
            case 'o':
                argStruct.outFile = optarg;
                break;
            case 'h':
                argStruct.flags[help] = true;
                break;
            case 'c':
                argStruct.flags[copy] = true;
                break;
            case 'v':
                argStruct.flags[verify] = true;
                break;
            case 'd':
                argStruct.flags[printDir] = true;
                break;
            case 'D':
                argStruct.delFile = optarg;
                argStruct.flags[deleteFile] = true;
                break;
            case 'x':
                argStruct.flags[extractFile] = true;
                argStruct.extractFile = optarg;
                break;
            case '?':
                if (optopt == 'D')
                {
                    argStruct.delFile = NULL;
                    argStruct.flags[deleteFile] = true;
                }
                else if (optopt == 'x')
                {
                    argStruct.extractFile = NULL;
                    argStruct.flags[extractFile] = true;
                }
                break;
        }
    }

    /* If the output file is NULL, then output is equal to input */
    if (argStruct.outFile == NULL)
    {
        argStruct.outFile = argStruct.inFile;
    }

    return argStruct;
}
