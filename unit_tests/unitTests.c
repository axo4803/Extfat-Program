#include "unitTests.h"

MunitResult test_initFileInfoStruct()
{
    // Runs bash script to create an image file
    system("bash ./examples/create_image.bash");
    char *filename = "test.image";
    fileInfo file = initFileInfoStruct(filename);

    // Check if critical values are set properly
    munit_assert_int(file.fd, !=, -1);
    munit_assert_ptr_not_equal(file.mainBoot, (void *)-1);
    munit_assert_string_equal(file.fileName, filename);

    // Deletes image file and unmounts
    system("rm test.image");
    system("bash ./tests/clean_up.bash");
    return MUNIT_OK;
}

MunitResult test_mmapCopy()
{
    // Runs bash script to create an image file
    system("bash ./examples/create_image.bash");

    fileInfo inputFileInfo = initFileInfoStruct("test.image");
    char *outputFilename = "test2.image";

    mmapCopy(&inputFileInfo, outputFilename);

    // diff returns 0 if both test.image and test2.image are the same
    int result = system("diff test.image test2.image");
    munit_assert_int(result, ==, 0); // Checks if diff returned a 0

    // Deletes image files and unmounts
    system("rm test.image test2.image");
    system("bash ./tests/clean_up.bash");
    return MUNIT_OK;
}

MunitResult test_writeByteInFile()
{
    char *outputFilename = "test_output.bin";
    size_t offset = 1 << 10;

    writeByteInFile(outputFilename, offset);

    FILE *fp = fopen(outputFilename, "rb");
    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);
    fclose(fp);

    // Check that file size is equal to the offset
    munit_assert_int(fileSize, ==, offset);

    // Check that byte at the offset is equal to 'B'
    fp = fopen(outputFilename, "rb");
    fseek(fp, offset - 1, SEEK_SET);
    char readByte;
    fread(&readByte, sizeof(char), 1, fp);
    fclose(fp);

    // Delete excess output files
    system("rm test_output.bin");
    munit_assert_char('B', ==, readByte);

    return MUNIT_OK;
}

MunitResult test_freeFileInfoStruct()
{
    // Runs bash script to create an image file
    system("bash ./examples/create_image.bash");
    fileInfo file = initFileInfoStruct("test.image");
    freeFileInfoStruct(&file);

    // Checks to see if freeFileInfoStruct set everything to zero
    munit_assert_null(file.mainBoot);
    munit_assert_null(file.backupBoot);
    munit_assert_null(file.FAT);
    munit_assert_null(file.Data);
    munit_assert_null(file.fileName);
    munit_assert_int(file.fd, ==, 0);
    munit_assert_int(file.SectorSize, ==, 0);
    munit_assert_int(file.SectorsPerCluster, ==, 0);
    munit_assert_int(file.FileLength, ==, 0);
    munit_assert_long(file.size, ==, 0);

    // Deletes image file and unmounts
    system("bash ./tests/clean_up.bash");
    return MUNIT_OK;
}

MunitResult test_verifyBoot()
{
    // Runs bash script to create an image file
    system("bash ./examples/create_image.bash");
    fileInfo file = initFileInfoStruct("test.image");

    // Calc checksums and check if they are the same or not
    uint32_t mbrChecksum = BootChecksum((uint8_t *)file.mainBoot, (short)file.SectorSize);
    uint32_t bbrChecksum = BootChecksum((uint8_t *)file.backupBoot, (short)file.SectorSize);
    uint32_t result = (mbrChecksum == bbrChecksum);

    // Check if verify comes out with the same input as above
    munit_assert_uint32(verifyBoot(&file), ==, result);

    // Deletes image file and unmounts
    system("bash ./tests/clean_up.bash");
    return MUNIT_OK;
}

MunitResult test_parseArgs()
{
    // Test no input value(s) (low bound)
    argument_struct_t argStruct1 = parseArgs(0, NULL);
    bool flagResultNone[NUM_FLAGS] = {false, false, false, false, false, false};

    // Check values of the argument struct
    munit_assert_null(argStruct1.inFile);
    munit_assert_null(argStruct1.outFile);
    munit_assert_null(argStruct1.delFile);
    munit_assert_null(argStruct1.extractFile);
    munit_assert_memory_equal(sizeof(flagResultNone), argStruct1.flags, flagResultNone);


    // Middle test
    char *argsPartial[] = {"./extfat",
                           "-i",
                           "test.image", // 2
                           "-cv",
                           NULL};
    argument_struct_t argStructPartial = parseArgs(4, argsPartial);
    bool flagResultPartial[NUM_FLAGS] = {false, true, true, false, false, false};

    // Check values of the argument struct
    munit_assert_ptr_equal(argStructPartial.inFile, argsPartial[2]);
    munit_assert_ptr_equal(argStructPartial.outFile, argsPartial[2]);
    munit_assert_ptr_equal(argStructPartial.outFile, argStructPartial.inFile);
    munit_assert_memory_equal(sizeof(flagResultPartial), argStructPartial.flags, flagResultPartial);


    // Test largest input value(s) (max bound)
    char *allArgs[] = {"./extfat",
                       "-i",
                       "test.image", // 2
                       "-o",
                       "test2.image", // 4
                       "-v",
                       "-c",
                       "-d",
                       "-h",
                       "-x",
                       "targetExtract", // 10
                       "-D",
                       "targetDelete", // 12
                       NULL};
    argument_struct_t argStruct2 = parseArgs(13, allArgs);
    bool flagResult2[NUM_FLAGS] = {true, true, true, true, true, true};

    // Check values of the argument struct
    munit_assert_ptr_equal(argStruct2.inFile, allArgs[2]);
    munit_assert_ptr_equal(argStruct2.outFile, allArgs[4]);
    munit_assert_ptr_equal(argStruct2.extractFile, allArgs[10]);
    munit_assert_ptr_equal(argStruct2.delFile, allArgs[12]);
    munit_assert_memory_equal(sizeof(flagResult2), argStruct2.flags, flagResult2);

    return MUNIT_OK;
}

MunitTest tests[] =
    {
        {"/test_initFileInfoStruct", test_initFileInfoStruct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/test_mmapCopy", test_mmapCopy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/test_writeByteInFile", test_writeByteInFile, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/test_freeFileInfoStruct", test_freeFileInfoStruct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/test_verifyBoot", test_verifyBoot, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {"/test_parseArgs", test_parseArgs, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
    };

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)])
{
    MunitSuite suite = {"/tests", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
    return munit_suite_main(&suite, NULL, argc, argv);
}
