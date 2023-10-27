#This bash script will check if the system produces the error message when attempting to delete a directory 

echo "Checks the error message when attempting to delete a directory"
bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
mkdir /tmp/d/dir1
sync
output=$(./extfat -i test.image -D dir1)
errorMessage="dir1 is a directory, unable to delete"
if [[ $output == *"$errorMessage"* ]]; then
    result=0
else
    result=1
fi
echo "Result is $result "
exit $result
