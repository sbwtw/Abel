#!/bin/bash
SRC=`pwd`
TMPDIR="$SRC/.tmp"
OUTDIR="$SRC/output"
IMG="$SRC/.tmp/image"
UBOOT="$SRC/uboot"
mkdir -p $TMPDIR
mkdir -p $OUTDIR
sdsize=16
dd if=/dev/zero bs=1M status=none count=$sdsize | pv -p -b -r -s $(( $sdsize * 1024 * 1024 )) | dd status=none of=${IMG}.raw
LOOP=$(losetup -f)
losetup $LOOP ${IMG}.raw

sudo dd if=$UBOOT/idbloader.bin of=$LOOP seek=64 conv=notrunc status=none >/dev/null 2>&1
sudo dd if=$UBOOT/uboot.img of=$LOOP seek=16384 conv=notrunc status=none >/dev/null 2>&1
sudo dd if=$UBOOT/trust.bin of=$LOOP seek=24576 conv=notrunc status=none >/dev/null 2>&1
mv ${IMG}.raw $OUTDIR/rock64_rk3328_uboot.img
rm -r $TMPDIR
echo 'success!'
