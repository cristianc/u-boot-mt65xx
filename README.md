# U-Boot for MemeTek MT6572
**In 2nd bootloader mode, packed to Android boot.img*

## Status
**Y** = works;\
**P** = partially works;\
**N** = does't work;\
**N/A** = untested / unknown.

* Booting = **N/A**;
* UART = **N/A**;
* Display (via simple-framebuffer) - **N/A**;

## Building
Just run the `build_mt6572.sh` script:
```
./build_mt6572.sh
```

## Flashing
You can use either `fastboot` or `mtkclient`, this guide uses the latter.
You need to power off the device, then enter: 
```
mtk w bootimg u-boot-mt6572.img
``` 
and connect the device to your PC with a USB cable.

