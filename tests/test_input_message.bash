#!/bin/bash

echo "Input message missing Test"

errorMessage="Error: missing '-i inputFile'
Try './extfat -h' for more information"
output=$(./extfat -i)
#Check to see if output contains the error message
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
