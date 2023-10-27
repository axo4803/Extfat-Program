#!/bin/bash

echo "Multiple Flags/Operations test"

# Create a disk image
bash ./tests/clean_up.bash
bash ./examples/create_image.bash
mkdir /tmp/d/dir1
touch /tmp/d/dir1/file
sync
#Copy Test 
CopyImage=($(/usr/bin/md5sum test.image))
./extfat -v -c -d -i test.image -o test2.image
CopyImage2=($(/usr/bin/md5sum test2.image))
output=$(./extfat -v -c -d -i test.image -o test2.image)
$(rm test2.image)
IntactOutput="Main Boot and Backup Boot checksums are the same"
# Check if the output contains the expected file names
if [[ $output == *"file"* ]] && [[ $output == *"mmap.c"* ]] && [[ $output == *"dir1"* ]] && [[ $output == *"$IntactOutput"* ]] ; then
    result=0
else
    result=1
fi
#Check if the two images are the same 
if [ "${CopyImage[0]}" = "${CopyImage2[0]}" ]; then
    result=$((result + 0))
else
    result=1
fi
#Now I mess up the image so main and backup boot should be differnet now
gcc -Wall examples/overwrite.c
./a.out
output2=$(./extfat -v -c -d -i test.image -o test2.image)
errorMessage="Main Boot and Backup Boot checksums are NOT the same"
if [[ $output2 == *"$errorMessage"* ]]; then
    result=$((result + 0))
else
    result=1
fi
rm ./a.out
echo "Result is $result"
exit $result
