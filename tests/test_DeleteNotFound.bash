#!/bin/bash
echo "Checks the error message when Delete does not detect a file"
bash ./tests/clean_up.bash 
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -D monkeyDluffy)
errorMessage="Unable to find monkeyDluffy"
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
