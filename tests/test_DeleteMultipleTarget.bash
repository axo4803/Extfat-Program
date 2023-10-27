echo "Delete multiple target Test"
#this test will test the case when there are two files with the same target name. The program shall perform operation on the first one it finds 

bash ./tests/clean_up.bash
bash ./examples/create_image.bash
#Inside tmp/d there is a mmap.c as indicated in examples/create_image.bash
mkdir /tmp/d/directory
touch /tmp/d/directory/mmap.c
cp /workspaces/cse3310_s004_group_18/examples/overwrite.c /tmp/d/directory/mmap.c
sync
sudo umount /tmp/d
#Unmount the image because We want the change to be reflected so we know which one is actually removed
output=$(./extfat -i test.image -D mmap.c)
#Now mount it back, the mmap.c with the same content as examples/mmap.c should be gone. 
sudo mount /dev/loop2 /tmp/d
#First of all, let's check that the first mmap.c is indeed gone
if [ -e /tmp/d/mmap.c ]; then
    result=1;
else
    result=0;
fi
#Check if the latter mmap.c is still there
if [ -e /tmp/d/directory/mmap.c ]; then
    result=$((result + 0))
else
    result=1;
fi
#Now let's double check that the mmap.c that remains is indeed the one we made after
diff /tmp/d/directory/mmap.c /workspaces/cse3310_s004_group_18/examples/overwrite.c
#When diff returns exit code 0 it means it matches
if [ $? -eq 0 ]; then
    echo "Files are identical"
    result=$((result + 0))
else
    echo "Files are different"
    result=1;
fi


echo "Result is $result"
exit $result