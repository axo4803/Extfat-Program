#pragma once

#include "directoryEntryInfo.h"

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo *clustInfo, int dirLevel);

/* Prints numBytes from cluster N in the exFAT file into an outfile. */
void printCluster(int fd, void *fp, FILE *outfile, int N, uint64_t numBytes, ClusterInfo *clustInfo);

/* Follows the FAT chain to be able to print all data from a file in the exFAT to an outfile.
 * Uses printCluster() to print data from a cluster to the outfile. */
void printAllClusterData(int fd, void *fp, FATChain *FAT, FILE *outfile,
                         StreamExt_t *streamExt, ClusterInfo *clustInfo);