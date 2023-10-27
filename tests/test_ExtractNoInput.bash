echo "System Extraction no input Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -o output.txt -x )
errorMessage="Missing target file to extract in test.image"
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result"
exit $result

