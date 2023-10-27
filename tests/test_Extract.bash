echo "System Extraction Accuracy Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -o output.txt -x mmap.c)
message="=== Extracting mmap.c from test.image into output.txt ==="
diff output.txt /workspaces/cse3310_s004_group_18/examples/mmap.c
#Diff returns a value of 0 when identical so I check if a return code of 0 is returned here
if [ $? -eq 0 ]  && [[ "$output" == *"$message"* ]]; then
    echo "Files are identical"
    result=0;
else
    echo "Files are different"
    result=1;
fi

rm output.txt
echo "Result is $result"
exit $result

