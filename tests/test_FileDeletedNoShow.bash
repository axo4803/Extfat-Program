echo "Deletion Check Test"
#This test checks that after -D is performed on the file, the file shall not show up when -d is ran
bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
./extfat -i test.image -D mmap.c

check=$(./extfat -i test.image -d)
#mmap.c should be deleted. If it shows up the we set result to 1 and fail the test. Otherwise we pass with result of 0
if [[ $output == *"mmap.c"* ]]; then
    result=1
else
    result=0
fi
echo "Result is $result"
exit $result
