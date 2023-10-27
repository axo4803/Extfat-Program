echo "Checks if system deletes"

bash ./tests/clean_up.bash
bash ./examples/create_image.bash 
sudo umount /tmp/d
output=$(./extfat -i test.image -D mmap.c)
message="=== Deleting mmap.c from test.image ===
Found mmap.c
mmap.c has been deleted"

if [ -e /workspaces/cse3310_s004_group_18/tmp/d/mmap.c ]  && [[ "$output" == *"$message"* ]]; then
    result=1
else
    result=0
fi

echo "Result is $result"
exit $result

