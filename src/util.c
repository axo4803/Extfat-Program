#include <stdint.h>

#include "util.h"
#include "extfat.h"

/* Prints out the help of how to use this program */
void printHelp()
{
    printf("HELP ON HOW TO USE\nNote: Input file (-i) required\n\n");
    printf("Help:\n\"-h\"\n\n");
    printf("Copy file:\n\"-c\"\n\n");
    printf("Verify file:\n\"-v\"\n\n");
    printf("Print directory:\n\"-d\"\n\n");
    printf("Extract a file:\n\"-x fileName\"\n\n");
    printf("Delete a file:\n\"-D fileName\"\n\n");
    printf("Input file (required):\n\"-i fileName\"\n\n");
    printf("Output file:\n\"-o fileName\"\n\n");
}

/* Initalizes a fileInfo struct with a given filename */
fileInfo initFileInfoStruct(char *fileName)
{
   fileInfo file = {};
   file.fd = open(fileName, O_RDWR);
   if (file.fd == -1)
   {
      perror("file open");
      exit(0);
   }

   struct stat statbuf;
   if (fstat(file.fd, &statbuf))
   {
      perror("stat of file");
      exit(0);
   }

   file.size = statbuf.st_size; // add size to the thing

   void *fp = (void *)mmap(NULL, file.size, PROT_READ | PROT_WRITE, MAP_PRIVATE, file.fd, 0);

   if (fp == (void *)-1)
   {
      perror("mmap");
      exit(0);
   }

   // first, is the Main Boot record
   Main_Boot *MB = (Main_Boot *)fp;
   file.mainBoot = MB;

   int bytesPerSector = 2 << (file.mainBoot->BytesPerSectorShift - 1); // Can be added to a property of the struct
   
   void *fp_ptr = (void*)(intptr_t)fp;
   file.backupBoot = (Main_Boot *)(fp_ptr + 12 * bytesPerSector);

   file.SectorSize = bytesPerSector;
   file.SectorsPerCluster = 2 << (MB->SectorsPerClusterShift - 1);

   file.FAT = (uint32_t *)(fp_ptr + (file.mainBoot->FatOffset * bytesPerSector));
   file.fileName = fileName;

   return file;
}

/* Unmaps the image file from memory and closes the corresponding file discriptor 
 * along with setting everything in the struct to zero/NULL */
void freeFileInfoStruct(fileInfo *file)
{
   // unmap the file
   if (munmap(file->mainBoot, file->size))
   {
      perror("error from unmap:");
      exit(0);
   }

   // close the file
   if (close(file->fd))
   {
      perror("close");
   }
   
   memset(file, 0, sizeof(fileInfo)); // set everything to zero/NULL
}