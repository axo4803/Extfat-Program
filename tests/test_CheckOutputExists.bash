#!/bin/bash
echo "Check Output Exist Test"
#Only checks that the destination file is created, does not check for accuracy, accuracy is done in a another test

# this should be a call to extfat -c <infile> <outfile>
./extfat -i test.image -o destination.image -c

if [ -e /workspaces/cse3310_s004_group_18/destination.image ]; then
    result=0
else
    result=1
fi
rm destination.image
echo "Result is $result "
exit $result
