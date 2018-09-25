#!/bin/bash
echo 'creating partition'
target=$1
parted -s $target -- mklabel msdo
parted -s $target -- mkpart primary fat16 32768s 4227071s
parted -s $target -- mkpart primary fat16 4227072s 16809983s
parted -s $target -- mkpart primary fat16 16809984s 29392895s
parted -s $target -- mkpart primary fat16 29392896s -1s

mkfs.ext4 -F ${target}1
mkfs.ext4 -F ${target}2
mkfs.ext4 -F ${target}3
mkfs.btrfs -f ${target}3

parted -l

echo 'dd uboot'
ubootDir="./assets/linux-u-boot-rock64_5.50_arm64"
dd if=$ubootDir/idbloader.bin of=$target seek=64 conv=notrunc status=none
dd if=$ubootDir/uboot.img of=$target seek=16384 conv=notrunc status=none
dd if=$ubootDir/trust.bin of=$target seek=24576 conv=notrunc status=none

sync

mkdir -p /tmp/partP /tmp/partA
mount ${target}1 /tmp/partP
mount ${target}2 /tmp/partA

echo 'copy boot script to partP'
cp -r ./assets/boot /tmp/partP/

echo 'extract rootfs to partA'
tar -xf ./output/abel-rootfs-emmc-base.tar.gz -C /tmp/partA/

echo 'create fstab'
echo "UUID=$(blkid -s UUID -o value ${target}2) / ext4 defaults,noatime,nodiratime,commit=600,errors=remount-ro 0 1" >> /tmp/partA/etc/fstab
echo "tmpfs /tmp tmpfs defaults,nosuid 0 0" >> /tmp/partA/etc/fstab

sync

umount /tmp/partP
umount /tmp/partA

echo 'done!'
