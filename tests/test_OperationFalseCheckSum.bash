echo "Bad Checksum test"
#This test ensures that the extfat utility does not operate other instructions except -h when having a bad checksum 
bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
gcc -Wall examples/overwrite.c
./a.out
output=$(./extfat -i test.image -v -d -D mmap.c -c -o output -x mmap.c)

#The program should ONLY print Main Boot and Backup Boot Checksums are not the same. -d -c -x should not have been executed or print anything 
if [[ $output == *"Main Boot and Backup Boot checksums are NOT the same"* ]]; then
    result=0
    if [[ $output == *"mmap.c"* || $output == *"Printing the directory listing of test.image"* || $output == *"DISCLAIMER: -c and -x are both active! Will only execute -c"* ]]; then
        result=1
    else 
        result=0
    fi
fi

rm ./a.out
echo "Result is $result "
exit $result
