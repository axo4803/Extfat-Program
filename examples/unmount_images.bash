#!/bin/bash
# This script will unmount an image
sudo umount /dev/loop2
sudo losetup -d /dev/loop2