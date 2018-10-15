#!/bin/bash
echo 'creating partition'
target=$1
parted -s $target -- mklabel msdo
parted -s $target -- mkpart primary fat16 65535s 131070s
parted -s $target -- mkpart primary fat16 131071s 12713982s
parted -s $target -- mkpart primary fat16 12713983s 25296894s
parted -s $target -- mkpart extended 25296895s -1s
parted -s $target -- mkpart logical 25296896s 33685503s
parted -s $target -- mkpart logical 33685505s -1s

mkfs.ext4 -F ${target}1
mkfs.ext4 -F ${target}2
mkfs.ext4 -F ${target}3
mkswap ${target}5
mkfs.btrfs -f ${target}6

parted -l

sync

mkdir -p /tmp/partP /tmp/partA
mount ${target}1 /tmp/partP
mount ${target}2 /tmp/partA

echo 'copy boot script to partP'
cp -r ./assets/boot /tmp/partP/

echo "rootdev=UUID=$(blkid -s UUID -o value ${target}2)" >> /tmp/partP/boot/envA.txt
echo "rootdev=UUID=$(blkid -s UUID -o value ${target}3)" >> /tmp/partP/boot/envB.txt

echo 'extract rootfs to partA'
tar -xf ./output/abel-rootfs-emmc-base.tar.gz -C /tmp/partA/

echo 'create fstab'
echo "UUID=$(blkid -s UUID -o value ${target}2) / ext4 defaults,noatime,nodiratime,commit=600,errors=remount-ro 0 1" >> /tmp/partA/etc/fstab
echo "UUID=$(blkid -s UUID -o value ${target}5) none swap sw 0 0" >> /tmp/partA/etc/fstab
echo "tmpfs /tmp tmpfs defaults,nosuid 0 0" >> /tmp/partA/etc/fstab

sync

umount /tmp/partP
umount /tmp/partA

echo 'done!'
