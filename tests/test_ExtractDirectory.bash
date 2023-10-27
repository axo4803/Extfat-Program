echo "Extract directory Test"
#Error message shall appear when attempting to extract directory

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
mkdir /tmp/d/dir1
sync
output=$(./extfat -i test.image -o output.txt -x dir1)
errorMessage="dir1 is a directory, unable to extract"
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
