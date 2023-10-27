echo "Extract multiple target Test"
#this test will test the case when there are two files with the same target name. The program shall perform operation on the first one it finds 

bash ./tests/clean_up.bash
bash ./examples/create_image.bash
#In ./examples/create_image.bash we have copied examples/mmap.c into /tmp/d
mkdir /tmp/d/directory
touch /tmp/d/directory/mmap.c
cp /workspaces/cse3310_s004_group_18/examples/overwrite.c /tmp/d/directory/mmap.c
sync
output=$(./extfat -i test.image -o output.txt -x mmap.c)
#Let's make sure that there are indeed two files with the same name 
if [ -e /tmp/d/mmap.c ] && [ -e /tmp/d/directory/mmap.c ] ; then
    result=0;
else
    result=1;
fi

#Now let's check that the first mmap.c is what is being extracted :)
diff /tmp/d/mmap.c output.txt
#When diff returns exit code 0 it means it matches
if [ $? -eq 0 ]; then
    echo "Files are identical"
    result=$((result + 0))
else
    echo "Files are different"
    result=1;
fi

rm output.txt
echo "Result is $result"
exit $result