# "Das U-Boot" for MediaTek MT6572/MT6580
## Supported devices
### MT6572
Lenovo A369i: U-Boot replaces the kernel;
### MT6580
Prestigio Multipad Wize 3151 (MT8321): U-Boot replaces the LK.

## Status
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
