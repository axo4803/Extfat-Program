#include "searchExtfat.h"
#include "deleteExtfat.h"

/* ===== Additional internal helper functions ===== */

/* Turn off the corresponding bit for cluster N in the bitmap. */
void turnOffCorrespondingBitInMap(int fd, void *fp, int N, uint8_t *allocBitMap)
{
    /* Each bit in the bitmap corresponds to a cluster.
     * Bit 0 - cluster 2
     * Bit 1 - cluster 3
     * ...
     * Bit 7 - cluster 9
     * Essentially, each byte references up to 8 clusters.
     *
     * This means that if cluster 20 is being freed and because allocBitMap
     * is a pointer to a byte, it needs to be "indexed" to the right section
     * of the bitmap.
     * N will also be used to set up a bitmask to properly flip the
     * correct bit, so it must be decreased to the 2-9 range.
     * Thus this while loop indexes to the proper section of the bitmap
     * and sets up N to be used to create the bitmask. */
    while (N > 9)
    {
        allocBitMap++; // Moves allocBitMap to the next byte

        /* Decreases the value of N by 8, since we've moved 8 bits forward in the bitmap
         * which is like shifting the starting cluster by 8 */
        N -= 8;
    }

    // Seeks to the proper location in the allocation bitmap in the image file
    lseek(fd, (off_t)((void *)allocBitMap - fp), SEEK_SET);

    /* If N = 2, then the 0th bit needs to be turned off because
     * Bit 0 - cluster 2, hence the -2 on N.
     *
     * Left bitshift is to ensure the correct bit will be flipped by XOR,
     * so if cluster 3 is cleared, then the 1st bit needs to be flipped in the bit mask.
     * cluster 7, then the 5th bit will need to be flipped.
     *
     * bitwise XOR is used to flip a specific bit, since it's being
     * used against 0b11111111, then it will always turn off a specific bit,
     * creating the proper bitmask to use against the allocBitMap.
     *
     * bitwise AND is used to always turn off a specific bit.
     * With the current bitmask, it will leave the other bits in allocBitMap untouched,
     * and flip only the bit that is 0 in the mask. */
    *allocBitMap &= (0b11111111 ^ (1 << (N - 2)));

    write(fd, allocBitMap, 1); // Write the changed value into the image file
}

/* Turns the InUse bit off of FileNameEntries and empties the data. */
void clearFileNameData(int fd, void *fp, FileNameEntry *firstEntry, int numOfFilenameEntries)
{
    /* An empty FileNameEntry used to copy over into the image file to
     * empty out the name of the file. */
    FileNameEntry emptyFileNameEntry = {};

    // FileName is 0xc1, the value is part of an enum defined in directoryEntryInfo.h
    emptyFileNameEntry.EntryType = FileName;
    emptyFileNameEntry.InUse = 0; // Turns off the InUse bit

    // Move to the target FileNameEntry in the image
    lseek(fd, (off_t)((void *)firstEntry - fp), SEEK_SET);

    // Goes through each FileNameEntry belonging to the file
    for (int i = 0; i < numOfFilenameEntries; i++)
    {
        // Writes the emptyFileNameEntry to the exFAT image file
        write(fd, &emptyFileNameEntry, sizeof(FileNameEntry));
    }
}

/* ===== End of Additional internal helper functions ===== */


/* ===== Functions declared in deleteFile.h ===== */

/* Clears the data in the Nth cluster in an exFAT image file and turns off the
 * corresponding bit in the allocation bitmap with turnOffCorrespondingBitInMap(). */
void clearCluster(int fd, void *fp, int N, ClusterInfo *clustInfo, uint8_t *allocBitMap)
{
    void *cluster = findCluster(N, fp, clustInfo); // Gets cluster N

    uint8_t zero = 0;                           // Byte of data to write
    lseek(fd, (off_t)(cluster - fp), SEEK_SET); // Seeks to cluster N in the image file

    // Clear the entire cluster to zeros
    for (size_t i = 0; i < clustInfo->bytesPerCluster; i++)
    {
        write(fd, &zero, 1);
    }

    turnOffCorrespondingBitInMap(fd, fp, N, allocBitMap);
}

/* Clears the FAT chain starting at FAT[index] along with the corresponding cluster data.
 * Uses ClearCluster() to clear the cluster data. */
void clearFATChainAndData(int fd, void *fp, FATChain *FAT, uint32_t index, ClusterInfo *clustInfo, uint8_t *allocBitMap)
{
    // Number of bytes from the start of the file to the FAT.
    off_t offset = (size_t)((void *)FAT - fp);

    uint32_t temp = index; // Used to remember the next FAT location to got to

    // This while loop runs until the end of the FAT chain is reached
    do
    {
        lseek(fd, offset + index * sizeof(FATChain), SEEK_SET); // Seeks to FAT[index]

        temp = FAT[index];       // Stores the current value
        FAT[index] = 0x00000000; // Wipes the FAT[index]

        write(fd, &FAT[index], sizeof(FATChain)); // Writes wiped value into the image file

        clearCluster(fd, fp, index, clustInfo, allocBitMap); // Clears the corresponding cluster

        index = temp; // sets up for the next FAT entry in the chain
    } while (index != 0xFFFFFFFF);
}

/* Flips the InUseBits of DirectoryEntries until it encounters the last
 * FileNameEntry. Uses clearFileNameData() to empty and turn off the InUse bit
 * of FileNameEntries. */
void turnOffInUseBits(int fd, void *fp, GDS_t *GDS, int numOfFilenameEntries)
{
    // Seek to the directory struct in the file
    lseek(fd, (off_t)((void *)GDS - fp), SEEK_SET);

    /* Note that the first directory should be a FileAndDirEntry (0x85), then
     * the one after that is StreamExtEntry (0xc0), which the one following that
     * is FileNameEntry (0xc1). When FileNameEntry is encountered, leave the
     * loop and use clearFileNameData() to handle the last entries corresponding
     * to the file to delete. */
    int i = 0;
    do
    {
        GDS[i].InUse = 0;                // Set the InUse bit to off
        write(fd, &GDS[i].EntryType, 1); // Write it into the file
        i++;                             // Move to the next directory struct

        /* shifts to the next directory struct in the file, -1 is
         * there to account for the offset from write() writing a byte */
        lseek(fd, sizeof(GDS_t) - 1, SEEK_CUR);
    } while (GDS[i].EntryType != FileName);

    // Clears and turns off InUse bits of the FileNameEntries
    clearFileNameData(fd, fp, (FileNameEntry *)&GDS[i], numOfFilenameEntries);
}

/* ===== End of Functions declared in deleteFile.h ===== */