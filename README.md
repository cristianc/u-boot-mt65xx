# "Das U-Boot" for MediaTek MT6572/MT6580
MT6572: In 2nd bootloader mode (replaces the kernel);\
MT6580: In 1st bootloader mode (replaces the `lk`).

## Status
**Y** = works;\
**P** = partially works;\
**N** = does't work;\
**N/A** = untested / unknown.

### MT6572
* Booting = **Y**;
* UART = **Y**;
* Display (via simple-framebuffer) - **Y**;

### MT6580
* Booting = **Y**;
* UART = **Y**;
* Display (via simple-framebuffer) - **N/A**;

## Building
1. `make` the defconfig:
```
make O=out mt6572_defconfig
```
2. Run the `build.sh` script:
```
./build.sh
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
4. Profit
