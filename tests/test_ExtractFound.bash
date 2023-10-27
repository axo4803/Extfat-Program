echo "System Extraction Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -o output.txt -x mmap.c)
message="Found mmap.c
mmap.c has been extracted to output.txt"
if [[ $output == *"$message"* ]]; then
    result=0
else
    result=1
fi
rm output.txt
echo "Result is $result"
exit $result

