#!/bin/bash
#Here I overwrite a byte in so Verify should return "Not the same" If it does then this test passes else it will not pass and return 1
echo "Verify Test"
bash ./tests/clean_up.bash
bash ./examples/create_image.bash
IntactSum=$(./extfat -i test.image -v)
message="=== Verifying the checksums of test.image ==="
if [[ $IntactSum == *"Main Boot and Backup Boot checksums are the same"* ]] && 
   [[ $IntactSum == *"$message"* ]]; then
    echo "Result is 0"
    result=0
else
    echo "Result is 1"
    result=1
fi

gcc -Wall examples/overwrite.c
./a.out
./extfat -i test.image -v
output=$(./extfat -i test.image -v)
errorMessage="Main Boot and Backup Boot checksums are NOT the same"
#Check to see if output contains the error message
if [[ $output == *"$errorMessage"* ]] && 
   [[ $output == *"$message"* ]]; then
    result=$((result + 0))
else
    result=1
fi
rm ./a.out
echo "Result is $result "
exit $result
