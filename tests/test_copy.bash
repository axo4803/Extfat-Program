#!/bin/bash
echo "Copy Files Test"

result1=($(/usr/bin/md5sum test.image))

#checks for the accuracy between the md5sum hashes of the two files. 
output=$(./extfat -i test.image -o test2.image -c)
message="=== Copying test.image to test2.image ==="
message2="Copied Successfully!"

result2=($(/usr/bin/md5sum test2.image))

# clean up
$(rm test2.image)

if [ "${result1[0]}" = "${result2[0]}" ] && [[ "$output" == *"$message"* ]] && [[ "$output" == *"$message2"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
