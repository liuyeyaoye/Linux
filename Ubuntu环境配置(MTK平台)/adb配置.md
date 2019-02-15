

```c
使用adb时，需要配置以下文件：
1、 /etc/udev/rules.d
（1）51-android.rules 的内容：（如果没有该文件则自行创建）
SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", MODE="0666"

（2）51-MTKinc.rules 的内容（如果没有该文件则自行创建）
SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", MODE="0666"
KERNEL=="ttyACM*", MODE="0666"

2.  ~/.android
（1）adb_usb.ini 的内容：（如果没有该文件则自行创建）
0x0e8d

（2）0e8d哪来的？
在终端中输入：lsusb，会有：
Bus 001 Device 029: ID 0e8d:2003 MediaTek Inc.

```

