#pragma once
/* This file contains struct, enum, and union definitions of what is used
 * to manipulate and find files in the extfat program. */

#include <stdint.h>
#include <stdlib.h>

typedef struct ClusterInfo
{
    uint32_t clustHeapOffs;
    uint32_t bytesPerSect;
    uint32_t sectPerCluster;
    size_t   bytesPerCluster;
} ClusterInfo;

typedef uint32_t FATChain; // Each entry in the FAT is 4 bytes

// These values are based on the Directory Entry - Entry Type subsection defined here
// http://elm-chan.org/docs/exfat_e.html
enum entryTypeCodes
{
    AllocBitM = 0x81, // Allocation bitmap entry
    FileDir   = 0x85, // File and Directory entry
    StreamExt = 0xc0, // Stream Extension  entry
    FileName  = 0xc1  // File Name entry
};

// The EntryType union and GenericDirectoryStruct are based on the directories defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#6-directory-structure
#define __UNION_ENTRY_TYPE__        \
union                               \
{                                   \
    uint8_t EntryType;              \
    struct                          \
    {                               \
        uint8_t TypeCode : 5;       \
        uint8_t TypeImportance : 1; \
        uint8_t TypeCategory : 1;   \
        uint8_t InUse : 1;          \
    };                              \
}

typedef struct GenericDirectoryStruct
{
    __UNION_ENTRY_TYPE__;
    uint8_t  CustomDefined[19];
    uint32_t FirstCluster;
    uint64_t DataLength;
} GDS_t;

// The Allocation Bitmap Entry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-us/windows/win32/fileio/exfat-specification#71-allocation-bitmap-directory-entry
typedef struct AllocationBitmapEntry
{
    __UNION_ENTRY_TYPE__;
    uint8_t  BitMapFlags;
    uint8_t  Reserved[18];
    uint32_t FirstCluster;
    uint64_t DataLength;
} AllocBitmapEntry;

// From 7.4 File Directory Entry, the FileAttributes has an offset of 4
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#74-file-directory-entry
#define FILE_ATTRIBUTE_OFFSET 4

// The FileAttributes struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#744-fileattributes-field
typedef struct FileAttributes
{
    uint16_t ReadOnly : 1;
    uint16_t Hidden : 1;
    uint16_t System : 1;
    uint16_t Reserved1 : 1;
    uint16_t Directory : 1;
    uint16_t Archive : 1;
    uint16_t Reserved2 : 10;
} FileAttributes;

// The StreamExtensionEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#76-stream-extension-directory-entry
typedef struct StreamExtensionEntry
{
    __UNION_ENTRY_TYPE__;
    union
    {
        uint8_t  GeneralSecondaryFlags;
        struct
        {
            uint8_t AllocationPossible : 1;
            uint8_t NoFatChain : 1;
            uint8_t Reserved : 6;
        };
    };
    uint8_t  Reserved1;
    uint8_t  NameLength;
    uint16_t NameHash;
    uint8_t  Reserved2[2];
    uint64_t ValidDataLength;
    uint8_t  Reserved3[4];
    uint32_t FirstCluster;
    uint64_t DataLength;
} StreamExt_t;

// The FileNameEntry struct is based on the directory entry defined here
// https://learn.microsoft.com/en-gb/windows/win32/fileio/exfat-specification#77-file-name-directory-entry
typedef struct FileNameEntry
{
    __UNION_ENTRY_TYPE__;
    uint8_t GeneralSecondaryFlags;
    uint8_t FileName[30];
} FileNameEntry;
