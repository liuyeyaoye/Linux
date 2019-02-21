


```c



1. addr2line 工具处理 AEE 错误
log中有 AEE 错误：
01-01 00:30:04.150   729   729 I AEE/AED : backtrace:
01-01 00:30:04.150   729   729 I AEE/AED :     #00 pc 0001d308  /system/bin/factory (_Z10psam_entryP9ftm_paramPv+143)
01-01 00:30:04.150   729   729 I AEE/AED :     #01 pc 000159f1  /system/bin/factory (ftm_entry+252)
01-01 00:30:04.150   729   729 I AEE/AED :     #02 pc 0000a115  /system/bin/factory (my_item_test_report+272)
01-01 00:30:04.151   729   729 I AEE/AED :     #03 pc 000081e5  /system/bin/factory (main+1384)
01-01 00:30:04.151   729   729 I AEE/AED :     #04 pc 00012471  /system/lib/libc.so (__libc_init+44)
01-01 00:30:04.151   729   729 I AEE/AED :     #05 pc 00009588  /system/bin/factory (_start+96)

将 prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-addr2line 复制到 /usr/bin 目录，
adb push /out/target/product/top8321_fgemmc/symbols/system/bin/factory /system/bin/factory
arm-linux-androideabi-addr2line -e factory 0001d308
可以定位到出错的行。



2. audit2allow 工具处理 selinux 的 AVC 错误
Ubuntu14.04 安装 audit2allow ：
which audit2allow ：查看使用的是哪个目录的 audit2allow 工具，然后删除它。
sudo apt-get install policycoreutils
which audit2allow
抓取开机的串口log，搜索 avc , 保存到 avc.txt。
sudo audit2allow -i avc.txt
如果提示： unable to open (null):  错误的地址，将 avc.txt 中的无效行删除即可。
然后会在终端输出要添加的权限，例如：
allow mtkbt su:unix_dgram_socket sendto;
然后在 selinux 目录 grep mtkbt , 将它添加进去。




```



