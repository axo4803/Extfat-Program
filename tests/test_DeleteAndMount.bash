echo "File Deleted After Mounting Test"
#This test will test the case when the file is deleted from the image if mounted/remounted, it will not show up with the ls command
bash ./tests/clean_up.bash 
bash ./examples/create_image.bash 
./extfat -i test.image -D mmap.c
sudo umount -q /tmp/d
sudo losetup -d /dev/loop2
sudo losetup /dev/loop2 test.image
sudo mount /dev/loop2 /tmp/d
output=$(ls /tmp/d)
#We use the ls command, if the output still contains mmap.c then we fail the test
if [[ $output == *"mmap.c"* ]]; then
    result=1
else
    result=0
fi
echo "Result is $result "
exit $result