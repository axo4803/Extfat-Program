echo "Operation C and X Test"
#When both -c and -x are used, only -c shall be performed

output=$(./extfat -i test.image -o output.txt -c -x mmap.c)

if [[ $output == *"DISCLAIMER: -c and -x are both active! Will only execute -c"* ]]; then
    result=0
else
    result=1
fi
rm output.txt
echo "Result is $result "
exit $result
