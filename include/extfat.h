#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stddef.h>

// This header defines the layout of data on an extfat disk image.
// For the details, please refer to
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification

typedef struct
{
    char JumpBoot[3];
    char FileSystemName[8];
    char MustBeZero[53];
    long int PartitionOffset;
    long int VolumeLength;
    int FatOffset;
    int FatLength;
    int ClusterHeapOffset;
    int ClusterCount;
    int FirstClusterOfRootDirectory;
    int VolumeSerialNumber;
    short int FileSystemRevision;
    short int VolumeFlags;
    unsigned char BytesPerSectorShift;
    unsigned char SectorsPerClusterShift;
    unsigned char NumberOfFats;
    unsigned char DriveSelect;
    unsigned char PercentInUse;
    unsigned char Reserved[7];
    unsigned char BootCode[390];
    short int BootSignature;
    unsigned char ExcessSpace;
} Main_Boot;

typedef struct
{
    // There are the disk image
    Main_Boot *mainBoot;
    Main_Boot *backupBoot;
    void *FAT;
    void *Data;
    // To identify the file
    char *fileName;
    int fd; // The file descriptor
    // For convenience, these values are extracted from the image
    int SectorSize; // in bytes
    int SectorsPerCluster;
    int FileLength;
    off_t size;
} fileInfo;
