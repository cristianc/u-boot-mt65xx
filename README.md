# "Das U-Boot" for MediaTek MT6572/MT6580
## Status
### Supported devices
#### MT6572
Lenovo A369i: U-Boot replaces the kernel;\
JTY D101: U-Boot replaces the LK(?).
#### MT6580
Prestigio Multipad Wize 3151 (MT8321): U-Boot replaces the LK.
#### MT6582
Alcatel OT-7041D: U-Boot replaces the kernel - quirky. See board commit message.
Huawei Honor Holly/3C Lite: U-Boot replaces the kernel. Probably same thing as the Alcatel, not sure.

**Y** = works;\
**P** = partially works;\
**N** = doesn't work.

### MT6572
* Booting - **Y**;
* UART - **Y**;
* Display - **Y** (simple-framebuffer);
* Internal storage / eMMC - **N**;
* External storage / SD card - **N**;
* Buttons - **N**;
* USB - **N**;

### MT6580
* Booting - **Y**;
* UART - **Y**;
* Display - **N**;
* Internal storage / eMMC - **Y**;
* External storage / SD card - **Y**;
* Buttons - **N**;
* USB - **N**;

### MT6582
* Booting - **Y**;
* UART - **Y**;
* Display - **Y** (simple-framebuffer);
* Internal storage / eMMC - **N**;
* External storage / SD card - **N**;
* Buttons - **N**;
* USB - **N**;

## Building
### Lenovo A369i (MT6572)
1. Run the `build_mt6572.sh` build script:
```
./build_mt6572.sh
```
2. Profit

### JTY D101 (MT6572)
1. Enter:
```
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out d101_defconfig
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out -j$(nproc --all)
```
2. Profit

### MT6580
1. Enter:
```
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out mt6580_defconfig
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out -j$(nproc --all)
```
2. Profit

### Alcatel OT-7041D (MT6582)
1. Run the `build_mt6582.sh` build script:
```
./build_mt6582.sh
```
2. Profit
### Honor Holly (MT6582)
1. Compile:
```
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out huawei_holly_defconfig.
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out -j$(nproc --all)
```
2. Pack into a boot.img:
```
dd if=/dev/random of=/tmp/ramdisk-dummy bs=2048 count=9
tools/mtk-mkimage ROOTFS /tmp/ramdisk-dummy /tmp/ramdisk-dummy.mtk
tools/mtk-mkimage KERNEL out/u-boot.bin /tmp/u-boot.bin.mtk
mkbootimg-osm0sis --kernel /tmp/u-boot.bin.mtk --ramdisk /tmp/ramdisk-dummy.mtk -o u-boot-mt6582.img
```
3. Profit

## Flashing
### MT6572
1. Power off the device
2. Enter:
```
mtk w bootimg u-boot-mt6572.img
```
3. Connect to the PC with a USB cable
4. Profit

### MT6580
1. Power off the device
2. Enter:
```
mtk w lk out/u-boot-mtk.bin
```
3. Connect to the PC with a USB cable
4. Profit

### MT6582
1. Power off the device
2. Run SP Flash Tool
3. Select scatter file, then **BOOTIMG** and choose u-boot-mt6582.img
4. Press **Download**
5. Connect to the PC with a USB cable
6. Profit
