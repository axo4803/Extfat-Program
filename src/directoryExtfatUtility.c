#include "directoryExtfatUtility.h"
#include "searchExtfat.h"
#include "printExtfat.h"
#include "deleteExtfat.h"
#include "routines.h"

#define START_DIR_LVL 1

/* Checks if main boot checksum is the same as the backup boot checksum */
int verifyBoot(fileInfo *file)
{
   uint32_t mbrChecksum = BootChecksum((uint8_t*) file->mainBoot, (short) file->SectorSize);
   uint32_t bbrChecksum = BootChecksum((uint8_t*) file->backupBoot, (short) file->SectorSize);
   
   return mbrChecksum == bbrChecksum;
}

/* Deletes a target file in the exFAT image.
 * - returns FOUND (1) when the file exists and is deleted
 * - returns NOT_FOUND (-1) if the file is not found
 * - returns DIRECTORY (0) if the file is a directory (does not delete it) */
int deleteFileInExfat(fileInfo *file, char *fileToDelete)
{
    /* Here to help make the code look cleaner and
     * reduce number of -> operations */
    int fd = file->fd;
    Main_Boot *MB = file->mainBoot;
    void *fp = (void *)file->mainBoot;

    // Contains information about cluster/sector size and offset location
    ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                             file->SectorSize * file->SectorsPerCluster};

    // Goes to the first directory entry
    GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, &clustInfo);

    // The allocation bitmap of the exFAT file, will be used when clearing cluster data
    uint8_t *allocBitMap = findAllocBitMap(GDS, fp, &clustInfo);

    // Finds the FileAndDirEntry (0x85) of the fileToDelete
    GDS_t *FileDirEntry = findFileAndDirEntry(GDS, fileToDelete, fp, &clustInfo);

    // Used to check if the target file is a directory
    FileAttributes *fileAttributes = (FileAttributes *)((void *)FileDirEntry + FILE_ATTRIBUTE_OFFSET);

    /* If FileDirEntry is NULL, then findFileAndDirEntry failed to find the file (return NOT_FOUND)
     * If the entry is a directory, then do not delete it and return DIRECTORY. */
    if (FileDirEntry == NULL)
    {
        return NOT_FOUND;
    }
    else if (fileAttributes->Directory)
    {
        return DIRECTORY;
    }

    /* If GDS is a FileAndDirectoryEntry(0x85 EntryType), then the
     * next entry after that is the StreamExtentionEntry. */
    StreamExt_t *streamExtEntry = (StreamExt_t *)(FileDirEntry + 1);

    /* Bit shift right by 4 is the same as integer division by 16.
     * There is always at least 1 filename entry, hence the +1.
     * One file name entry is large enough to hold 15 characters max, so if a filename
     * has 16 characters, then that means there are two filename entries. The first
     * entry contains 15 characters, the second entry contains the 16th character. */
    int numOfFilenameEntries = (streamExtEntry->NameLength >> 4) + 1;

    // Check if the file has a FirstCluster. If the value is zero, then there is no cluster or Fat chain.
    if (streamExtEntry->FirstCluster != 0)
    {
        /* Check if NoFatChain is false (equal to 0) to determine to clear the FAT chain
         * or just clear a single cluster */
        if (!streamExtEntry->NoFatChain)
        {
            clearFATChainAndData(fd, fp, file->FAT, streamExtEntry->FirstCluster, &clustInfo, allocBitMap);
        }
        else // there is only one cluster of file data
        {
            clearCluster(fd, fp, streamExtEntry->FirstCluster, &clustInfo, allocBitMap);
        }
    }

    /* turnOffInUseBits starts at the FileAndDirEntry, and turns off InUse bits
     * of all directory entries until it hits the last FileNameEntry. */
    turnOffInUseBits(fd, fp, FileDirEntry, numOfFilenameEntries);

    return FOUND;
}

/* Prints the entire directory list of the files in the exFAT image file */
void printAllDirectoriesAndFiles(fileInfo *file)
{
    /* Here to help make the code look cleaner and
     * reduce number of -> operations */
    Main_Boot *MB = file->mainBoot;
    void *fp = (void *)file->mainBoot;

    // Contains information about cluster/sector size and offset location
    ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                             file->SectorSize * file->SectorsPerCluster};

    // Goes to the first directory entry entry
    GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, &clustInfo);

    // Prints the directory listing of the exFAT image
    printDirectory(GDS, fp, &clustInfo, START_DIR_LVL);
}

/* Extracts a target file in the exFAT image to an output file.
 * - returns FOUND (1) when the target exists and is extracted 
 * - returns NOT_FOUND (-1) if the target file is not found
 * - returns DIRECTORY (0) if the target file is a directory (does not extract data) */
int extractFileInfo(fileInfo *file, char *targetFile, char *outputFilename)
{
    /* Here to help make the code look cleaner and
     * reduce number of -> operations */
    int fd = file->fd;
    Main_Boot *MB = file->mainBoot;
    void *fp = (void *)file->mainBoot;

    // Contains information about cluster/sector size and offset location
    ClusterInfo clustInfo = {MB->ClusterHeapOffset, file->SectorSize, file->SectorsPerCluster,
                             file->SectorSize * file->SectorsPerCluster};

    // Goes to the first directory entry
    GDS_t *GDS = findCluster(MB->FirstClusterOfRootDirectory, fp, &clustInfo);

    // Finds the FileAndDirEntry (0x85) of the fileToDelete
    GDS_t *FileDirEntry = findFileAndDirEntry(GDS, targetFile, fp, &clustInfo);

    // Used to check if the target file is a directory
    FileAttributes *fileAttributes = (FileAttributes *)((void *)FileDirEntry + FILE_ATTRIBUTE_OFFSET);

    /* If FileDirEntry is NULL, then findFileAndDirEntry failed to find the file (return NOT_FOUND)
     * If the entry is a directory, then do not extract data and return DIRECTORY. */
    if (FileDirEntry == NULL)
    {
        return NOT_FOUND;
    }
    else if (fileAttributes->Directory)
    {
        return DIRECTORY;
    }

    /* If GDS is a FileAndDirectoryEntry(0x85 EntryType), then the
     * next entry after that is the StreamExtentionEntry. */
    StreamExt_t *streamExtEntry = (StreamExt_t *)(FileDirEntry + 1);

    FILE *outputFile = fopen(outputFilename, "w");

    // Check if the file has a FirstCluster. If the value is zero, then there is no cluster or Fat chain.
    if (streamExtEntry->FirstCluster != 0)
    {
        /* Check if NoFatChain is false (equal to 0) to determine to clear the FAT chain
         * or just clear a single cluster */
        if (!streamExtEntry->NoFatChain)
        {
            printAllClusterData(fd, fp, file->FAT, outputFile, streamExtEntry, &clustInfo);
        }
        else // there is only one cluster of file data
        {
            printCluster(fd, fp, outputFile, streamExtEntry->FirstCluster, 
                         streamExtEntry->ValidDataLength, &clustInfo);
        }

    }

    fclose(outputFile);

    return FOUND;
}
