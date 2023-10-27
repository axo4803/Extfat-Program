#!/bin/bash

echo "Help Files Test"

# Call extfat with -h option and capture its output
output=$(./extfat -h)

# Define the expected help message
helpMessage="HELP ON HOW TO USE
Note: Input file (-i) required

Help:
\"-h\"

Copy file:
\"-c\"

Verify file:
\"-v\"

Print directory:
\"-d\"

Extract a file:
\"-x fileName\"

Delete a file:
\"-D fileName\"

Input file (required):
\"-i fileName\"

Output file:
\"-o fileName\""

# Check whether the output matches the expected help message
if [[ $output == "$helpMessage" ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
