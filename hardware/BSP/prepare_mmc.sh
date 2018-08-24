#!/bin/bash
target=$1
parted -s $target -- mklabel msdo
parted -s $target -- mkpart primary fat16 32768s 4227071s
parted -s $target -- mkpart primary fat16 4227072s 16809983s
parted -s $target -- mkpart primary fat16 16809984s 29392895s

mkfs.ext4 -F ${target}1
mkfs.ext4 -F ${target}2
mkfs.ext4 -F ${target}3

parted -l
echo 'done!'
