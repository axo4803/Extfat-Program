#!/bin/bash
# create the disk image file
dd if=/dev/zero of=test.image count=1 bs=$ImageSize
sudo losetup /dev/loop2 test.image
sudo /usr/sbin/mkexfatfs /dev/loop2
# mount the drive (may not always be needed)
mkdir /tmp/d
sudo mount /dev/loop2 /tmp/d

mkdir /tmp/d/dir1
touch /tmp/d/dir1/file1
sync