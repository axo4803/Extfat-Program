#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extfat.h"

/* Initalizes a fileInfo struct with a given filename */
fileInfo initFileInfoStruct(char *fileName);

/* Prints out the help of how to use this program */
void printHelp();

/* Unmaps the image file from memory and closes the corresponding file discriptor 
 * along with setting everything in the struct to zero/NULL */
 void freeFileInfoStruct(fileInfo *file);
