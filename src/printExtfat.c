#include "searchExtfat.h"
#include "printExtfat.h"

/* ===== Additional internal helper functions/Macros ===== */

#define MIN(x, y) (x <= y) ? x : y

/* Prints the filename inside the exFAT image with the proper amount of tabs
 * depending on the dirLevel. */
void printFileName(char *ptrToFilename, int lengthOfName, int dirLevel)
{
    // Prints the proper number of tabs dependidng on dirLevel
    for (int i = 0; i < dirLevel; i++)
    {
        printf("\t");
    }

    // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
    char *filename = malloc((lengthOfName + 1) * sizeof(char));
    fetchNameFromExtFAT(filename, ptrToFilename, lengthOfName);

    printf("%s\n", filename);
    free(filename);
}

/* ===== End of Additional internal helper functions/Macros ===== */


/* ===== Functions declared in printExtfat.h ===== */

/* Recursively traverses the exFAT image directory entries to print all files
 * and directories out in a tabbed format. Uses printFileName() to print the names. */
void printDirectory(GDS_t *GDS, void *fp, ClusterInfo *clustInfo, int dirLevel)
{
    int i = 0;
    while (GDS[i].EntryType)
    {
        // Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
        // and the next GenericDirectoryStructure (i+1) is a StreamExtEntry (StreamExt - 0xc0)
        // and the one after that (i+2) is the FileNamEntry (FileName - 0xc1)
        if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i + 1].EntryType == StreamExt && GDS[i + 2].EntryType == FileName)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExt_t *streamExtEntry = (StreamExt_t *)&GDS[i + 1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i + 2];

            printFileName((char *)fileNameEntry->FileName, streamExtEntry->NameLength, dirLevel);

            // If the attribute of the file is a directory, then recursively call this function to print its
            // contents, using its corresponding cluster, and increase the directory level
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                printDirectory(subGDS, fp, clustInfo, dirLevel + 1);
            }
        }

        i++;
    }
}

/* Prints numBytes from cluster N in the exFAT file into an outfile. */
void printCluster(int fd, void *fp, FILE *outfile, int N, uint64_t numBytes, ClusterInfo *clustInfo)
{
    void *cluster = findCluster(N, fp, clustInfo); // Gets cluster N

    lseek(fd, (off_t)(cluster - fp), SEEK_SET); // Seeks to cluster N in the image file

    fwrite(cluster, 1, numBytes, outfile); // Writes numBytes from the cluster into the outputfile
}

/* Follows the FAT chain to be able to print all data from a file in the exFAT to an outfile.
 * Uses printCluster() to print data from a cluster to the outfile. */
void printAllClusterData(int fd, void *fp, FATChain *FAT, FILE *outfile, StreamExt_t *streamExt, ClusterInfo *clustInfo)
{
    uint32_t index = streamExt->FirstCluster; // Setup first location to traverse through in the FAT

    // The total number of bytes of data there is in a file, so it will be the total number of bytes to write.
    uint64_t numBytes = MIN(streamExt->ValidDataLength, streamExt->DataLength);
    // NOTE: ValidDataLength should be less than DataLength, but just in case, take the MIN of the two.

    // This while loop runs until the end of the FAT chain is reached or there are no more bytes to write.
    do
    {
        /* If there are more bytes of data in the file than there are in a cluster, only
         * print the number of bytes in the cluster, and decrease the amount of bytes left to write. 
         * the rest of file data should be in the other clusters */
        if(numBytes > clustInfo->bytesPerCluster)
        {
            printCluster(fd, fp, outfile, index, clustInfo->bytesPerCluster, clustInfo);
            numBytes -= clustInfo->bytesPerCluster;
        }
        else
        {
            // Write the rest of the bytes left, and set numBytes to 0
            printCluster(fd, fp, outfile, index, numBytes, clustInfo);
            numBytes = 0;
        }

        index = FAT[index]; // sets up for the next FAT entry in the chain
    } while (index != 0xFFFFFFFF && numBytes != 0);
}

/* ===== End of Functions declared in printExtfat.h ===== */