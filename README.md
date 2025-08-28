# "Das U-Boot" for MediaTek MT6572
# Devices that this was tested with
* JTY D101
* ...

## Status
### What works
* Basic booting and relocation
* UART

### What doesn't
* Everything else

## Installation
### Building
```
ARCH=arm CROSS_COMPILE=arm-none-eabi- make mt6572_defconfig
ARCH=arm CROSS_COMPILE=arm-none-eabi- make -j$(nproc --all)
```

### Flashing to the device
The easiest way is to use `mtkclient`:
```
mtk w uboot u-boot-mtk.bin
```
