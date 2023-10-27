#!/bin/bash
echo "Check to See if the system exits after help"

# this should be a call to extfat -c <infile> <outfile>
./extfat -h

if [ $? -eq 0 ]; then
    result=0;
else 
    result=1;
fi
echo "Result is $result "
exit $result
