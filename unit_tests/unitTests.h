#pragma once

#include "munit/munit.h"
#include "copyExtfat.h"
#include "directoryExtfatUtility.h"
#include "extfat.h"
#include "parseArgs.h"
#include "routines.h"
#include "util.h"

fileInfo initFileInfoStruct(char *fileName);
MunitResult test_initFileInfoStruct();
MunitResult test_mmapCopy();

MunitResult test_writeByteInFile();
void writeByteInFile(char *outputFilename, size_t offset);
MunitResult test_freeFileInfoStruct();
MunitResult test_verifyBoot();
MunitResult test_parseArgs();
