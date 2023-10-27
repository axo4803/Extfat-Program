// #include <stdio.h>
// #include <string.h>

#include "extfat.h"
#include "parseArgs.h"
#include "copyExtfat.h"
#include "directoryExtfatUtility.h"
#include "util.h"

int main(int argc, char *argv[])
{
    // Collects argument info given by the user on execution
    argument_struct_t arguments = parseArgs(argc, argv);

    /* ===== Help (-h) ===== */
    if (arguments.flags[help] == true)
    {
        printHelp();
        return EXIT_SUCCESS;
    }
    else if (arguments.inFile == NULL)
    {
        printf("Error: missing \'-i inputFile\'\n"
               "Try \'./extfat -h\' for more information\n");
        return EXIT_FAILURE;
    }
    /* ===== End of Help (-h) ===== */

    // Has information about the exFAT image file
    fileInfo inputFileInfo = initFileInfoStruct(arguments.inFile);

    /* ===== Verify (-v) ===== */
    if (arguments.flags[verify] == true)
    {
        printf("\n=== Verifying the checksums of %s ===\n", arguments.inFile);
        if (verifyBoot(&inputFileInfo) == 1)
        {
            printf("Main Boot and Backup Boot checksums are the same\n");
        }
        else
        {
            printf("Main Boot and Backup Boot checksums are NOT the same\n");
            return EXIT_FAILURE;
        }
    }
    /* ===== End of Verify (-v) ===== */


    /* ===== Copy (-c) / Extract (-x) ===== */
    if (arguments.flags[copy] == true && arguments.flags[extractFile] == true)
    {
        printf("\nDISCLAIMER: -c and -x are both active! Will only execute -c");
    }

    if (arguments.flags[copy] == true) // Copy (-c)
    {
        printf("\n=== Copying %s to %s ===\n", arguments.inFile, arguments.outFile);
        if (mmapCopy(&inputFileInfo, arguments.outFile) != -1)
        {
            printf("Copied Successfully!\n");
        }
    }
    else if (arguments.flags[extractFile] == true) // Extract (-x)
    {
        if (arguments.extractFile == NULL) // Checks if a target file to extract is specified
        {
            printf("\n=== Missing target file to extract in %s ===\n", arguments.inFile);
        }
        else if (strcmp(arguments.outFile, arguments.inFile) == 0) // Ensure that input will not be overwriten
        {
            printf("\n=== Missing output file or output is same as input file ===\n");
        }
        else
        {
            printf("\n=== Extracting %s from %s into %s ===\n", arguments.extractFile, 
                   arguments.inFile, arguments.outFile);
            switch (extractFileInfo(&inputFileInfo, arguments.extractFile, arguments.outFile))
            {
                case FOUND:
                    // Target file found and extracted
                    printf("Found %s\n%s has been extracted to %s\n", arguments.extractFile, 
                           arguments.extractFile, arguments.outFile);
                    break;
                case DIRECTORY:
                    // Target file is a directory
                    printf("Found %s\n%s is a directory, unable to extract\n", 
                           arguments.extractFile, arguments.extractFile);
                    break;
                case NOT_FOUND:
                    // Target file does not exist
                    printf("Unable to find %s\n", arguments.extractFile);
                    break;
            }
        }
    }
    /* ===== End of Copy (-c) / Extract (-x) =====*/

    /* ===== Delete File (-D) ===== */
    if (arguments.flags[deleteFile] == true)
    {
        if (arguments.delFile == NULL) // Checks if a target file to delete is specified
        {
            printf("\n=== Missing target file to delete in %s ===\n", arguments.inFile);
        }
        else
        {
            printf("\n=== Deleting %s from %s ===\n", arguments.delFile, arguments.inFile);
            switch (deleteFileInExfat(&inputFileInfo, arguments.delFile))
            {
                case FOUND:
                    // Target file found and deleted
                    printf("Found %s\n%s has been deleted\n", arguments.delFile, arguments.delFile);
                    break;
                case DIRECTORY:
                    // Target file is a directory
                    printf("Found %s\n%s is a directory, unable to delete\n", 
                           arguments.delFile, arguments.delFile);
                    break;
                case NOT_FOUND:
                    // Target file does not exist
                    printf("Unable to find %s\n", arguments.delFile);
                    break;
            }
        }
    }
    /* ===== End of Delete File (-D) ===== */

    /* ===== Print Directory (-d) ===== */
    if (arguments.flags[printDir] == true)
    {
        printf("\n=== Printing the directory listing of %s ===\n", arguments.inFile);
        printAllDirectoriesAndFiles(&inputFileInfo);
    }
    /* ===== Print Directory (-d) ===== */

    printf("\n");
    freeFileInfoStruct(&inputFileInfo);

    return EXIT_SUCCESS;
}
