#pragma once

#include "extfat.h"

#define FOUND 1
#define DIRECTORY 0
#define NOT_FOUND -1

/* Checks if main boot checksum is the same as the backup boot checksum */
int verifyBoot(fileInfo *file);

/* Prints the entire directory list of the files in the exFAT image file */
void printAllDirectoriesAndFiles(fileInfo *file);

/* Deletes a target file in the exFAT image.
 * - returns FOUND (1) when the file exists and is deleted 
 * - returns NOT_FOUND (-1) if the file is not found
 * - returns DIRECTORY (0) if the file is a directory (does not delete it) */
int deleteFileInExfat(fileInfo *file, char *fileToDelete);

/* Extracts a target file in the exFAT image to an output file.
 * - returns FOUND (1) when the target exists and is extracted 
 * - returns NOT_FOUND (-1) if the target file is not found
 * - returns DIRECTORY (0) if the target file is a directory (does not extract data) */
int extractFileInfo(fileInfo *file, char *targetFile, char *outputfilename);
