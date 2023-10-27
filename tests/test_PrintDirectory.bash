#!/bin/bash

echo "Directory Test"

# Create a disk image

# Display the directory of the disk image

bash ./tests/clean_up.bash
bash ./examples/create_image.bash
mkdir /tmp/d/dir1
touch /tmp/d/dir1/file
sync
./extfat -i test.image -d 
#I put mmap.c dir1 and file inside the image
output=$(./extfat -i test.image -d)
message="=== Printing the directory listing of test.image ==="

# Check if the output contains the expected file names
if [[ $output == *"file"* ]] && [[ $output == *"mmap.c"* ]] &&
   [[ $output == *"dir1"* ]] && [[ $output == *"$message"* ]]; then
    result=0
else
    result=1
fi

echo "Result is $result"
exit $result
