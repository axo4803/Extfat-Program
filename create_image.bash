#/bin/bash
# this script is designed to make images to assist in developing and
# testing the program(s)
dd if=/dev/zero of=test.image count=1 bs=100M
sudo losetup /dev/loop2 test.image
sudo /usr/sbin/mkexfatfs /dev/loop2
#
#
#
# 
mkdir /tmp/d
sudo mount /dev/loop2 /tmp/d
cp examples/mmap.c /tmp/d
sync