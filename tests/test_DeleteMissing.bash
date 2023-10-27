echo "System Delete no input Test"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 

output=$(./extfat -i test.image -D)
errorMessage="Missing target file to delete in test.image"
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result"
exit $result

