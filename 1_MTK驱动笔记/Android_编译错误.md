
```c

vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:99: error: undefined reference to '__android_log_print'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:118: error: undefined reference to '__android_log_print'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:145: error: undefined reference to '__android_log_print'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:172: error: undefined reference to '__android_log_print'
解决：Android.mk中加上：LOCAL_SHARED_LIBRARIES += libcutils


vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:95: error: undefined reference to 'dlopen'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:98: error: undefined reference to 'dlerror'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:114: error: undefined reference to 'dlsym'
vendor/mediatek/proprietary/hardware/mtkcam/acdk/entry_mdk/entry_mdk.cpp:117: error: undefined reference to 'dlerror'
解决：Android.mk中加上：LOCAL_SHARED_LIBRARIES += libdl


vendor/ageing/stress/stress.cpp:48: error: undefined reference to 'ui_fill'
vendor/ageing/stress/stress.cpp:49: error: undefined reference to 'ui_flip'
vendor/ageing/stress/stress.cpp:52: error: undefined reference to 'ui_color'
vendor/ageing/stress/stress.cpp:53: error: undefined reference to 'ui_fb_width'
vendor/ageing/stress/stress.cpp:53: error: undefined reference to 'ui_fb_height'
vendor/ageing/stress/stress.cpp:53: error: undefined reference to 'ui_fill'
vendor/ageing/stress/stress.cpp:54: error: undefined reference to 'ui_flip'
collect2: error: ld returned 1 exit status
解决：Android.mk中加上：LOCAL_STATIC_LIBRARIES += libminiui



target Executable: factory (out/target/product/top8321_fgemmc/obj/EXECUTABLES/factory_intermediates/LINKED/factory)
vendor/mediatek/proprietary/factory/src/test/ftm_btdemo.cpp:369: error: undefined reference to 'OSSocketInitClientBt(TW_OSSocketDomain, OS_bt_callbacks_t*)'
vendor/mediatek/proprietary/factory/src/test/ftm_btdemo.cpp:384: error: undefined reference to 'OSSocketInitServerBt(TW_OSSocketDomain, OS_bt_callbacks_t*)'
collect2: error: ld returned 1 exit status
解决：在OSSocket.h中添加：
int OSSocketInitServerBt(TW_OSSocketDomain domain, OS_bt_callbacks_t * os_bt_callbacks_t);
int OSSocketInitClientBt(TW_OSSocketDomain domain, OS_bt_callbacks_t * os_bt_callbacks_t);



编译报错：
make:进入目录'/work2/1-POS/MB416/MB416_factory'
target Executable: testclock (out/target/product/top8321_fgemmc/obj/EXECUTABLES/testclock_intermediates/LINKED/testclock)
vendor/USDK/libusdk/clock/test.cpp:276: error: undefined reference to 'property_set'
collect2: error: ld returned 1 exit status
make: *** [out/target/product/top8321_fgemmc/obj/EXECUTABLES/testclock_intermediates/LINKED/testclock] 错误 1
make:离开目录“/work2/1-POS/MB416/MB416_factory”

解决：
Android.mk中添加  LOCAL_SHARED_LIBRARIES := libcutils
代码中添加头文件： #include <cutils/properties.h>



蓝牙send data失败：
I/AEE/AED (  851): backtrace:
I/AEE/AED (  851):     #00 pc 0003de88  /system/lib/libc.so (sendto+20)
I/AEE/AED (  851):     #01 pc 00017be1  /system/lib/libc.so (send+8)
I/AEE/AED (  851):     #02 pc 00011da1  /system/lib/libusdk.so (OSSend+44)
I/AEE/AED (  851):     #03 pc 00058305  /system/bin/factory
I/AEE/AED (  851):     #04 pc 0005367f  /system/bin/factory
I/AEE/AED (  851):     #05 pc 0004264d  /system/bin/factory (main+1744)
I/AEE/AED (  851):     #06 pc 00012471  /system/lib/libc.so (__libc_init+44)
I/AEE/AED (  851):     #07 pc 00043f90  /system/bin/factory
E/        (  237): [TAG] fail to send data to fd: 0.  len=-1
原因：蓝牙bonded失败引起的。



target Executable: factory (out/target/product/top8321_fgemmc/obj/EXECUTABLES/factory_intermediates/LINKED/factory)
vendor/mediatek/proprietary/factory/src/test/ftm_btdemo.cpp:195: error: undefined reference to 'BtSspReply(unsigned char*, int, int, int)'
collect2: error: ld returned 1 exit status
make: *** [out/target/product/top8321_fgemmc/obj/EXECUTABLES/factory_intermediates/LINKED/factory] 错误 1
原因：因为C语言中没有 bool 类型。



client bt出现AEE错误：
Abort message: 'FORTIFY_SOURCE: memcpy: prevented write past end of buffer. Calling abort()
#08 pc 000113d5  /system/lib/libusdk.so (_ZL38bt_remote_device_properties_callback_t11bt_status_tP11bt_bdaddr_tiP13bt_property_t+252)
解决：
定位：bt_remote_device_properties_callback_t()函数的memcpy出错：
将
uint8_t uuid[16];
memcpy((uint8_t *)uuid, (uint8_t *)ptr->val, len);
改成：
uint8_t uuid[128];
memcpy(uuid, (uint8_t *)ptr->val, len);


```

