# "Das U-Boot" for MediaTek MT6572/MT6580
MT6572: In 2nd bootloader mode (replaces the kernel);\
MT6580: In 1st bootloader mode (replaces the `lk`).

## Status
**Y** = works;\
**P** = partially works;\
**N** = doesn't work.

### MT6572
* Booting = **Y**;
* UART = **Y**;
* Display - **Y** (simple-framebuffer);
* Internal storage / eMMC - **N**;
* External storage / SD card - **N**;
* Buttons - **N**;
* USB - **N**;

### MT6580
* Booting = **Y**;
* UART = **Y**;
* Display (via simple-framebuffer) - **N**;
* Internal storage / eMMC - **Y**;
* External storage / SD card - **N**;
* Buttons - **N**;
* USB - **N**;

## Building
### MT6572
1. Run the `build_mt6572.sh` build script:
```
./build_mt6572.sh
```
2. Profit

### MT6580
1. `make` the defconfig:
```
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out mt6580_defconfig
```
2. Build `U-Boot` itself:
```
ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- make O=out -j$(nproc --all)
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
