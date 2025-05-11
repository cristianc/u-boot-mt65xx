#!/bin/bash

rm_prev_file() {
  prev_file="u-boot-mt6572.img"
  if [ -f $prev_file ]; then
    echo "previous u-boot image found, removing..."
    rm $prev_file
  else
    echo "previous u-boot image not found..."
  fi
}

build_uboot() {
  echo "building u-boot..."
  ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out a369i_defconfig
  ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out -j$(nproc --all)
  echo "u-boot build is done!"
}

make_android_bootimg() {
  echo "creating a dummy ramdisk..."
  dd if=/dev/random of=/tmp/ramdisk-dummy bs=2048 count=9
  echo "prepending the mtk rootfs header to dummy ramdisk..."
  tools/mtk-mkimage ROOTFS /tmp/ramdisk-dummy /tmp/ramdisk-dummy.mtk
  
  echo "prepending the mtk kernel header to u-boot image..."
  tools/mtk-mkimage KERNEL out/u-boot.bin /tmp/u-boot.bin.mtk

  echo "creating an android boot.img..."
  mkbootimg-osm0sis --kernel /tmp/u-boot.bin.mtk --ramdisk /tmp/ramdisk-dummy.mtk -o u-boot-mt6572.img
  echo "done!"
  echo "filename: u-boot-mt6572.img"
}

tmp_cleanup() {
  echo "cleaning up..."
  rm /tmp/ramdisk-dummy
  rm /tmp/ramdisk-dummy.mtk
  rm /tmp/u-boot.bin.mtk
  echo "done!"
}

main() {
  rm_prev_file
  build_uboot
  make_android_bootimg
  tmp_cleanup
}

main
