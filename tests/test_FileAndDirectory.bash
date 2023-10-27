echo "File and Directory Test"
#This test will create a file and a direcotry with the same name. The program shall perform the operation on the NONDIRECTORY 

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
mkdir /tmp/d/dir1
mkdir /tmp/d/dir1/dir1
cp examples/overwrite.c /tmp/d/dir1/dir1
mv /tmp/d/dir1/dir1/overwrite.c /tmp/d/dir1/dir1/dir1
sync
#We have dir1 which has subdir dir1 which has a file called dir1 that has overwrite.c content inside. We will then use -D to see if the system could go through the directories and locate the file

output=$(./extfat -i test.image -o output.txt -x dir1)
output1=$(md5sum examples/overwrite.c | cut -f1 -d' ')
output2=$(md5sum output.txt | cut -f1 -d' ')
if [[ $output == *"Found dir1"* ]]; then
    result=0;
    echo "Found dir1"
else
    echo "Not Found"
    result=1;
fi

if [ "$output1" == "$output2" ]; then
    echo "hash matches."
    result=$((result + 0))

else
    echo "hash do not match."
    result=1;
fi
#When we do delete we check to see if stdout contains the following messages
delete=$(./extfat -i test.image -D dir1)
if [[ "$delete" == *"Found dir1"* && "$delete" == *"dir1 has been deleted"* ]]; then
    echo "Delete part success"
    result=$((result + 0))
else
    echo "Delete not success"
    result=1
fi
echo "Result is $result"
exit $result
