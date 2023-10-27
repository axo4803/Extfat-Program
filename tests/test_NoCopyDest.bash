#!/bin/bash
echo "If no -o is specified then the extfat utility will not copy"

output=$(./extfat -i test.image -c)
outputMessage="Same input and output file name, nothing will happen."

if [[ $output == *"$outputMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
