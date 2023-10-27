#pragma once

#include "directoryEntryInfo.h"

/* Clears the data in the Nth cluster in an exFAT image file and turns off the
 * corresponding bit in the allocation bitmap */
void clearCluster(int fd, void *fp, int N, ClusterInfo *clustInfo, uint8_t *allocBitMap);

/* Clears the FAT chain starting at FAT[index] along with the corresponding cluster data. */
void clearFATChainAndData(int fd, void *fp, FATChain *FAT, uint32_t index, ClusterInfo *clustInfo, uint8_t *allocBitMap);

/* Flips the InUseBits of DirectoryEntries until it encounters the last
 * FileNameEntry. */
void turnOffInUseBits(int fd, void *fp, GDS_t *GDS, int numOfFilenameEntries);