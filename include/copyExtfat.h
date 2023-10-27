#pragma once

#include "extfat.h"

/* Takes in an input and output file. And uses mmap() to copy the contents from the
 * input file into the output file, and returns 0 if it is a success and a -1 otherwise */
int mmapCopy(fileInfo *inputFileInfo, char *output_file);
