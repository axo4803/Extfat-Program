#pragma once

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "directoryEntryInfo.h"

/* Finds the Nth Cluster in the exFAT image file */
void *findCluster(int N, void *fp, ClusterInfo *c);

/* Copies the filename from FileNameEntries into dest.
 * dest should have enough space to store lengthOfName characters +1 for '\0'. */
void fetchNameFromExtFAT(char *dest, char *ptrToFilename, int lengthOfName);

/* Finds the FileAndDirEntry (Entry Type = 0x85) of a target file/directory (fileToFind)
 * Returns NULL if there is no file or directory with the target file name.
 *
 * If the file with the target name found is a directory, continue searching until a
 * non-directory file with the target is found. If there is none, then return the directory.
 *
 * If there are multiple files with the target name, then return the first non-directory
 * file encountered. */
GDS_t *findFileAndDirEntry(GDS_t *GDS, char *fileToFind, void *fp, ClusterInfo *clustInfo);

/* Finds the Allocation BitMap Location in the exFAT image file */
uint8_t *findAllocBitMap(GDS_t *GDS, void *fp, ClusterInfo *clustInfo);
