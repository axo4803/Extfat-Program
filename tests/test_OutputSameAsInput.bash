#!/bin/bash

echo "Output same as Input for -x Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -o test.image -x mmap.c)
errorMessage="=== Missing output file or output is same as input file ==="
#Check to see if output contains the error message
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
