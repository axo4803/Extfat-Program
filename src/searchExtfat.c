#include <string.h>

#include "searchExtfat.h"

/* Finds the Nth Cluster in the exFAT image file */
void *findCluster(int N, void *fp, ClusterInfo *c)
{
    return ((fp + c->clustHeapOffs * c->bytesPerSect) + ((N - 2) * c->bytesPerSect * c->sectPerCluster));
}

/* Copies the filename from FileNameEntries into dest.
 * dest should have enough space to store lengthOfName characters +1 for '\0'. */
void fetchNameFromExtFAT(char *dest, char *ptrToFilename, int lengthOfName)
{
    for (int i = 0; i < lengthOfName; i++)
    {
        // In the event that the name is longer than 15 characters, then the next byte
        // that ptrToFilename will point at will be 0xc1(FileName) to signal another FileNameEntry,
        // thus must offset by 2 to get to the next printable character.
        if (*ptrToFilename == (char)FileName)
        {
            ptrToFilename += 2;
        }

        sprintf(dest, "%c", *ptrToFilename); // Prints the character into the destination string

        ptrToFilename += 2; // offset by 2 to get to the next character in the name
        dest++;             // move pointer to write to the next character
    }

    *dest = '\0'; // Writes '\0' to the end of the string
}

/* Finds the FileAndDirEntry (Entry Type = 0x85) of a target file/directory (targetFile)
 * Returns NULL if there is no file or directory with the target file name.
 *
 * If the file with the target name found is a directory, continue searching until a
 * non-directory file with the target is found. If there is none, then return the directory.
 *
 * If there are multiple files with the target name, then return the first non-directory
 * file encountered. */
GDS_t *findFileAndDirEntry(GDS_t *GDS, char *targetFile, void *fp, ClusterInfo *clustInfo)
{
    // The intended value to return. Is not NULL when a non-directory file of the target is found
    GDS_t *returnVal = NULL;

    // The backup value to return. Is not NULL when a directory file of the target is found
    GDS_t *backupVal = NULL;

    int i = 0;
    /* While loop keeps running until the target file that is not a 
     * directory is found, or until there are no more entries. */
    while (GDS[i].EntryType && returnVal == NULL)
    {
        /* Checks if if current GDS is FileAndDirectoryEntry (FileDir - 0x85)
         * and the next GenericDirectoryStructure (i+1) is a StreamExtEntry (StreamExt - 0xc0)
         * and the one after that (i+2) is the FileNamEntry (FileName - 0xc1) */
        if (GDS[i].InUse && GDS[i].EntryType == FileDir && GDS[i+1].EntryType == StreamExt && GDS[i+2].EntryType == FileName)
        {
            FileAttributes *fileAttributes = (FileAttributes *)((void *)&GDS[i] + FILE_ATTRIBUTE_OFFSET);
            StreamExt_t *streamExtEntry = (StreamExt_t *)&GDS[i+1];
            FileNameEntry *fileNameEntry = (FileNameEntry *)&GDS[i+2];

            // Gets the filename from exFAT into a standard C string. +1 is for '\0'.
            char *currFilename = malloc((streamExtEntry->NameLength + 1) * sizeof(char));
            fetchNameFromExtFAT(currFilename, (char *)fileNameEntry->FileName, streamExtEntry->NameLength);

            // Checks to see if the filename is equal to the targetFile
            if (strcmp(currFilename, targetFile) == 0)
            {
                // If the file is a directory, save the entry in backupVal
                if (fileAttributes->Directory)
                {
                    backupVal = &GDS[i];
                }
                else // The file is not a directory
                {
                    // Save the entry in returnVal, and prepare to leave the loop
                    returnVal = &GDS[i];
                }
            }

            /* If a directory is encountered, recusively call this function to
             * search through that directory for the target file. */
            if (fileAttributes->Directory)
            {
                GDS_t *subGDS = findCluster(streamExtEntry->FirstCluster, fp, clustInfo);
                subGDS = findFileAndDirEntry(subGDS, targetFile, fp, clustInfo);
                FileAttributes *subGDSAttributes = (FileAttributes *)((void *)subGDS + FILE_ATTRIBUTE_OFFSET);

                /* If subGDS is NULL then there was not a filename match found, 
                 * otherwise subGDS is the FileAndDirectoryEntry (FileDir - 0x85). 
                 *
                 * If subGDS is NOT a directory, then set returnVal equal to subGDS, and leave the loop */ 
                if (subGDS != NULL && !subGDSAttributes->Directory)
                {
                    returnVal = subGDS;
                }
                else if (backupVal == NULL) // if backupVal is NULL, set it equal to result of the recursion
                {
                    /* Note: If subGDS is NULL, then nothing changes.
                     * If subGDS is not NULL then a filename match was found, and 
                     * subGDS is a directory. Therefore, backupVal will be the entry
                     * of a directory with the same name as the target file. */
                    backupVal = subGDS;
                }
            }

            free(currFilename);
        }

        i++;
    }

    /* If returnVal is NULL, then a non-directory of the targetFile was not found, so
     * return the backupVal. If backupVal is NULL, then the targetFile did not exist. */
    return (returnVal) ? returnVal : backupVal;
}

/* Finds the Allocation BitMap Location in the exFAT image file */
uint8_t *findAllocBitMap(GDS_t *GDS, void *fp, ClusterInfo *clustInfo)
{
    // Searches for the AllocBitMapEntry
    int i = 0;
    while (GDS[i].EntryType != AllocBitM)
    {
        i++;
    }

    return (uint8_t *)findCluster(GDS[i].FirstCluster, fp, clustInfo);
}
