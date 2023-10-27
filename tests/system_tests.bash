#!/bin/bash
echo "Running system tests."

# first, need to make sure the system is in a consistent state,
# meaning no extfat drives are mounted, and /dev/loop2 does not exist
# it will also delete a "test.image" file, it it exists

bash "tests/clean_up.bash"
TestCount=0
TestFailed=0
TestPassed=0

for ImageSize in "1M" "5M" "10M"  
do
     for TestScript in $(ls tests/test_*.bash)
     do
        TestCount=$((TestCount+1))

        echo ">>>>>>>>>  $TestScript <<<<<<<<   $ImageSize  <<<<<<<<<<"
        export ImageSize
        bash "tests/create_disc_image.bash"


        bash $TestScript
        if [ $? -eq 0 ]
        then
            TestPassed=$((TestPassed+1))
        else
            TestFailed=$((TestFailed+1))
        fi
   
     # after the test completes, need to clean up
     bash "tests/clean_up.bash"
     done
done
echo "== Test Summary =="
echo "Total tests run: $TestCount"
echo "Tests passed: $TestPassed"
echo "Tests failed: $TestFailed."