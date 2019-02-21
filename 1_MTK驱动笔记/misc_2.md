

```c

Android O platfrom 分支代码：
repo_google init -u http://1.2.3.4:8080/xxxx/platform/manifest.git -m xxxx-O1-platform-dev.xml
repo_google sync -j4
repo_google start xxxx-O1-platform-dev --all


(2)安装openjdk-8:
sudo add-apt-repository ppa:openjdk/ppa
sudo add-apt-repository ppa:openjdk-r/ppa
sudo apt-get update
sudo apt-get install openjdk-8-jdk

sudo chmod -R 755 /usr/lib/jvm/java-8-openjdk-amd64

sudo gedit ~/.bashrc
文件末尾添加如下描述信息：
#for openJDK:
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export JRE_HOME=/usr/lib/jvm/java-8-openjdk-amd64/jre
export PATH=$JAVA_HOME/bin:$JAVA_HOME/jre/bin:$PATH
export CLASSPATH=$CLASSPATH:.:$JAVA_HOME/lib:$JAVA_HOME/jre/lib

. ~/.bashrc

sudo update-alternatives --config java
sudo update-alternatives --config javac
选择需要使用的JDK版本。

确认一下路径：
printenv JAVA_HOME
java -version



(3)编译
source ./build/envsetup.sh 
lunch full_xxxx_bsp-eng 
make -j8 2>&1 |tee build.log

3、DDR 路径： preloader/tools/emigen/MT6739/MemoryDeviceList_MT6739.xls

12、编译camera模块：

#mmm vendor/mediatek/proprietary/custom/mt6739/hal
#mmm vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr
#mmm vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/common/paramsmgr
#rm libcam.paramsmgr.so

mmma vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/common/paramsmgr
cd out/target/product/k39tv1_bsp/vendor/lib/

mmm vendor/mediatek/proprietary/packages/apps/Camera
adb push libcam.paramsmgr.so /vendor/lib


mmma vendor/mediatek/proprietary/platform/mt65XX/hardware/mtkcam/v1/common/paramsmgr


feature table静态库会被编译到libcam.paramsmgr.so中。

touch vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/feature/custom/custom.cpp && \
mmm vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/ 2>&1 | tee ft.lib.log && \
mmm vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/common/paramsmgr 2>&1 | tee ft.so.log

adb shell stop media && adb shell start media

touch vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/feature/custom/custom.cpp 
mmma vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/
mmma vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/common/paramsmgr 

mmma vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6735m/v1/common/paramsmgr

cd out/target/product/gernal/system/vendor/lib/
ls libcam.paramsmgr.so
adb push libcam.paramsmgr.so /system/vendor/lib

修改 HAL层 sensor.list 编译方法：
mmma vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6735m/hal/sensor
adb push libcam.halsensor.so /system/vendor/lib


13、
regulator_get 获取的是 supply 之前的名字，而不是 regulator-name = "vcamio";
vcamio_sub-supply = <0x85>;
vcamaf-supply = <0x86>;

15、合并代码在win7下进行，不要在Ubuntu中进行，因为Ubuntu会合并，而win7才是替换。

17、关于Linux命令
1）at 命令
它用于定时执行某一脚本

/etc/init.d/atd start
ps -ef | grep atd

例如：
at 14:30 -f ./time
atq
atrm 3

2）sudo script kmsg1.log
exit

3）grep -E "aaa|bbb"


16、adb命令大全：

手机的存储空间在 adb shell 中的路径为：/sdcard/

49. 屏幕截图
adb shell /system/bin/screencap -p /sdcard/screenshot.png 
adb pull /sdcard/screenshot.png ./screenshot.png

50. 从手机中获取 mtklog
adb pull /storage/emulated/0/mtklog ./mklog3/

51.安装/卸载应用程序，-r表示替换原来的 apk
adb install -r 应用程序.apk
adb uninstall xxx.apk

52.发送文件到手机中
adb push test.c /sdcard/test

23
6739平台 使用adb shell没有权限时：
error: insufficient permissions for device

sudo ~/bin/adb kill-server
sudo ~/bin/adb devices

59、 adb 亮屏和灭屏
adb shell input keyevent 26


60、 adb 调试振子
1000毫秒 = 1秒
adb shell  echo 1000 > /sys/devices/virtual/timed_output/vibrator/enable"

24、打开 camera 命令：
adb shell input keyevent 23
点击某处坐标 adb shell input tap x  y 
左滑		adb shell input swipe 664 930 45 930
右滑		adb shell input swipe 45 930 664 930

53.打印 log并输出到文件：查看上层log信息:
 adb logcat |grep "AAL"
 adb logcat > ./log.txt
 adb logcat > ./camera.txt

adb 录制屏幕：
adb shell 
screenrecord /sdcard/demo.mp4			//180s

adb shell screenrecord  --time-limit 10 /sdcard/demo.mp4	//10s

adb shell screenrecord --size 1280*720 /sdcard/demo.mp4

adb pull /sdcard/demo.mp4

55.adb 进入工程模式
adb shell am start -n com.mediatek.engineermode/.EngineerMode

Android 8.1默认开启dm-verify  ，  userdebug版本adb remount重启，需要替换本地默认adb为Android8.1编译出来 out/host/linux-x86/bin/adb文件，
adb root后执行adb disable-verity后重启机器，然后才能adb remount 修改system、vendor分区文件：
1、	adb root
2、	adb remount
3、	adb disable-verity
4、	adb reboot
经过上面4个步骤之后userdebug版本才可以允许adb push 

56.如何控制振动器
adb shell
echo 3000 > /sys/devices/virtual/timed_output/vibrator/enable
震动3秒

59、 编写 dts 文件注意：
内容离顶端1个或者2 个tab 键,（from M_branch_OTP_porting_guide.pdf   page 8）

18、关于内核线程的一个bug
1）在FT8006m的TP驱动中，它的i2c probe函数是通过mtk tp probe调用的，而mtk tp probe函数是通过一个工作队列实现的。
2）FT8006m的固件升级函数，是通过一个工作队列实现的。
3）二者的工作处于不同的工作队列，分别由内核线程调度执行。
4）二者执行的先后顺序不一样，所以二者不能通过变量相联系。（如 extern ）

19、当在 Gerrit 上 abandon 后，怎么撤销提交：
		git log .
		git reset --hard commitID
		git pull
		git status

20、 i2c的几种传输：
kernel中：
i2c_transfer()
i2c_master_send
i2c_smbus_write_byte_data

22、
取代码出错：  	fatal: ssh variant 'simple' does not support setting port
解决：  				git config --global ssh.variant ssh

23.如果一个相同的camera ，有2种，后缀为 b 的为后摄，后缀为 s 的为前摄。
ov8856b			后摄
OV9760    		后摄
ov8856			后摄
OV9760s 		前摄

24.
git升级：
sudo apt-get add-apt-repository ppa:git-core/ppa
sudo apt-get update
sudo apt-get install git

9、GPIO配置可以这样读出来：
adb shell cat /sys/devices/platform/1000b000.pinctrl/mt_gpio

 [MODE] [DIR] [DOUT] [DIN] [PULL_EN] [PULL_SEL] [IES] [SMT] [DRIVE]
  95: 110010101
  96: 110010101
 112: 110010101

23、 Linux V4L2 是针对 UVC(usb视频规范)规范的，即USB摄像头。UVC 驱动路径为： kernel-3.10/drivers/media/usb/uvc/
MTK标准的Camera并没有采用v4l2框架，所以需要在jni层实现基本的v4l2视频采集流程。

27、YuSu.cmp
vendor/mediatek
proprietary/bootable/bootloader/lk/scripts/dct/config/YuSu.cmp
proprietary/bootable/bootloader/preloader/tools/dct/config/YuSu.cmp
proprietary/scripts/dct/YuSu.cmp
proprietary/scripts/dct/python/config/YuSu.cmp


29.
lcd enter sleep:
0x9001,0x28,
0xa000,5,
0x9001,0x10,
0xa000,120

lcd_exist_sleep:
0x9001,0x11,
0xa000,120,
0x9001,0x29,
0xa000,5

33、 DB文件位置：
/data/vendor/mtklog/aee_exp
/data/aee_exp 
/sdcard/mtklog/aee_exp
/data/anr

Android O变化：
Android O ,mtklog 和db 不在同一个目录，提交log 时需要同时导出来：
adb pull /sdcard/mtklog
adb pull /data/aee_exp
adb pull /data/vendor/mtklog/aee_exp

35、关闭TSF
/vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor/gc5025s_mipi_raw/camera_tuning_para_gc5025smipi_raw.cpp
isTsfEn

adb shell "echo 0 > /sys/module/printk/parameters/disable_uart" 

重启查看log：
 cat /proc/last_kmsg
adb logcat -v threadtime
 echo 8 > /proc/sys/kernel/printk  		//打印所有log
cat proc/bootprof

39、 strings 用法
在 .so 文件中查看它编译了哪个 .cpp  文件： XXXX为代码中的某个字符串
strings -f "*.so" | grep "xxxxxx"
strings cam.so

40、加打印语句时，这样才能打印出来：
printk("xxx %s line=%d\n",__FUNCTION__,__LINE__);

45、
请您帮忙分别dump后摄sensor方向90和270时pass2 data:

adb root
adb shell setenforce 0
adb shell setprop camera.dumpbuffer.enable 2


46、 adb 获取 mtklog 
adb pull /storage/emulated/0/mtklog ./mklog10/


47、camera调试：
ov8856b			后摄
OV9760    		后摄
ov8856			后摄
OV9760s 		前摄

48、imgsensor 旋转问题：
1）修改 s5k3l8mipiraw_Sensor.c  的  set_mirror_flip(IMAGE_NORMAL);	
2）修改 .sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_Gr,

3）修改 vendor 目录的 cfg_setting_imgsensor.cpp

4）平板项目修改 vendor 目录的 imgsensor_metadata

51、at命令
at 01:00 -f ~/work/test

52、git pull 时，出现头指针分离于 0adad89afda 时，使用下面2条命令：
repo_google sync
repo_google start xxxx-driver-dev --all

56、 alsps
ps_data > ps_threshold_high : close			ps_data_report! 0, 3    接近
ps_data < ps_threshold_low : far away		ps_data_report! 1, 3   远离

9、GPIO配置可以这样读出来：
adb shell cat /sys/devices/platform/1000b000.pinctrl/mt_gpio

  1 PIN: [MODE] [DIR] [DOUT] [DIN] [PULL_EN] [PULL_SEL] [IES] [SMT] [DRIVE] ( [R1] [R0] )
  95: 110010101
  96: 110010101
 112: 110010101
 6: 000000110
119: 011100100 00

61、当配置成NC的gpio实际开机后是个什么状态？
通常是Mode 0，GPIO Mode，输入模式，使能内部下拉。

62、
在进行i2c 传输前设置，i2c_client->timing = timing_number;
当设备不支持快速		mode timing_number <= 100;
当设备支持快速			mode timing_number <= 400;
普通模式:100kHz
快速模式：400kHz
高速模式：3.4MHz
没有任何必要使用高速SCL，将SCL保持在100k或以下；

63、如何拉低并保持 LCM RESET PIN 脚为低 电平
平台默认 RESET PIN 脚输出为高平的，如果一定需要拉低，可以配置RESET PIN脚为GPIO模式，再通过GPIO方式拉低。
使用mediatek/dct目录下的DCT工具，使用其打开custom/XXX/kernel/dct/dct目录下的DWS文件，将GPIO131配置成 0:GPIO131. 
对GPIO PIN脚的控制有如下一些方法：
lcm_util.set_gpio_mode(GPIO131, GPIO_MODE_00);
lcm_util.set_gpio_dir(GPIO131,GPIO_DIR_OUT);
lcm_util.set_gpio_out(GPIO131,0);


64、DSI接口有两种sync 模式：video mode和command mode
其中video mode是BB端一直刷数据到LCM，cmd mode是在有数 据更新时刷数据到LCM GRAM中)
和DSI command mode相比，video mode 是需要实时传输image data到lcm端，DSI 的refresh rate决定了lcm的refresh rate。


66、虚拟机中USB和串口线不能同时连接。

67、
ret =  tee_spi_transfer(&smt_conf, cfg_len, writebuf, readbuf, 7);
在指纹驱动中，这个函数有最大调用次数限制，
以前6737平台上最多能调用20次，现在6739平台不知道能调用多少次，


ISP register 显示 seninf 没有收到sensor 发来的数据：
[0x15008134 00000000]

69、camera otp权限文件 CAM_CAL_DRV

device/mediatek/mt6739/init.mt6739.rc
device/mediatek/sepolicy/basic/non_plat/cameraserver.te
device/mediatek/sepolicy/basic/non_plat/device.te
device/mediatek/sepolicy/basic/non_plat/file_contexts
device/mediatek/sepolicy/basic/non_plat/mediaserver.te
device/mediatek/sepolicy/basic/non_plat/mtk_hal_camera.te
device/mediatek/sepolicy/basic/prebuilts/api/26.0/nonplat_sepolicy.cil
device/mediatek/sepolicy/bsp/non_plat/atci_service.te
mediatek/sepolicy/bsp/prebuilts/api/26.0/nonplat_sepolicy.cil
mediatek/sepolicy/full/prebuilts/api/26.0/nonplat_sepolicy.cil

70、gc5025 反转180度/镜像设置

#if defined(GC5025_MIRROR_NORMAL)
.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_R,//sensor output first pixel color
#elif defined(GC5025_MIRROR_H)	//上下翻转
.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_Gr,//sensor output first pixel color
#elif defined(GC5025_MIRROR_V)		 //左右翻转
.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_Gb,//sensor output first pixel color
#elif defined(GC5025_MIRROR_HV)		//上下左右翻转
.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,//sensor output first pixel color
#else
.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_R,//sensor output first pixel color
#endif


71、 LSC
1)平台端otp LSC导通验证：
编译eng版本；
格式化下载；
adb shell logcat > lsc.log 	抓第一次开机log
2）遇到的问题
没有执行imx214_camera_calibration_cam_cal.cpp这个文件，而是执行了camera_calibration_cam_cal.cpp这个文件
解决：添加 imx214的dev权限。

3)、lsc golden 数据前面68个数据要注释掉，只要后面1800个数据

4) PixId:    0,      //first pixel:其中{0,2，4,8} 分别对应 {B,Gb,Gr,R},
配置pixel id.通过和 module厂商确定烧录的OTP的first pixel


grep -rn madev ./mediatek/

72、W5528迈瑞微指纹：
hardware/interfaces/biometrics/fingerprint/2.1/default/BiometricsFingerprint.cpp
hardware/libhardware/hardware.c

     <hal format="hidl">
        <name>android.hardware.biometrics.fingerprint</name>
        <transport>hwbinder</transport>
        <version>2.1</version>
        <impl level="generic"></impl>
        <interface>
            <name>IBiometricsFingerprint</name>
            <instance>default</instance>
        </interface>
     </hal>


adb logcat -v time



修改了 vendor 目录，只编译 systemimage，按照下面这种方法行不通。
find . -type f -name "*cam*"
find . -type f -name "*cam*" -delete
source ./build/envsetup.sh
lunch
make -j8 systemimage
刷机时刷写 system.img 和 vendor.img。
修改了HAL层，正确的应该是：
删除product目录，全部编译。


rm out/target/product/top6739_36_n1/system/vendor/lib/libcam.paramsmgr.so
rm out/target/product/top6739_36_n1/obj/SHARED_LIBRARIES/libcam.paramsmgr_intermediates/LINKED/libcam.paramsmgr.so


//for MT8321
rm out/target/product/top8321_fgemmc/system/lib/libcam.paramsmgr.so && rm out/target/product/top8321_fgemmc/system/lib/libcameracustom.so
touch vendor/mediatek/proprietary/hardware/mtkcam/v1/common/paramsmgr/feature/custom/custom.cpp
source ./build/envsetup.sh
lunch
mmma -j4 vendor/mediatek/proprietary/hardware/mtkcam/v1/common/paramsmgr/
//grep libcameracustom.so ./ -nr
//grep libcam.paramsmgr.so ./ -nr
mmma -j4 vendor/mediatek/proprietary/platform/mt6580/hardware/mtkcam/v1/common/paramsmgr 2>&1 |tee camhal.log
//end

//for MT6739
rm out/target/product/k39tv1_bsp/vendor/lib/libcam.paramsmgr.so && rm out/target/product/k39tv1_bsp/vendor/lib/libcameracustom.so
touch vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/feature/custom/custom.cpp 
source ./build/envsetup.sh
lunch
mmma  vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/
mmm  vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/common/paramsmgr 
mmm  vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6761/v1/common/paramsmgr
mmm  vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6580/v1/common/paramsmgr 2>&1 |tee camhal.log
//end

Android 8.1 push 方法：
重启手机
cd out/target/product/k39tv1_bsp/vendor/lib/
ls libcam.paramsmgr.so -l && ls -l libcameracustom.so
adb root && adb remount
adb push libcam.paramsmgr.so ./vendor/lib/
adb push libcameracustom.so ./vendor/lib/
//注意64位系统要push到lib64
//adb shell sync
//adb shell rm ./vendor/nvdata/media/CAMERA*
//adb shell rm ./system/vendor/nvdata/media/CAMERA*
重启手机
//恢复出厂设置



Android 7.1 push 方法：    //Android 7.1 push 2次 进去后重启可以生效。
首先重启机器；
adb root && adb remount
adb push libcam.paramsmgr.so ./system/vendor/lib/
然后重启机器；


Android 5.1 push方法：
adb root && adb remount
cd out/target/product/top8321_fgemmc/system/lib
ls libcam.paramsmgr.so -l && ls -l libcameracustom.so
adb push libcam.paramsmgr.so /system/lib/libcam.paramsmgr.so
adb push libcameracustom.so /system/lib/libcameracustom.so
adb push libacdk.so /system/lib/libacdk.so
rm /data/nvram/APCFG/APRDCL/CAM*
adb shell sync
adb reboot

在adb push一个文件到机器中，先将这个修改的文件touch一下，如
touch factory.ini
adb push factory.ini /etc/factory.ini
rm /data/nvram/APCFG/APRDCL/CAM*



一、镜像功能：

1、相关的目录
frameworks/
hardware/
packages/
vendor/mediatek/proprietary/packages/apps/

vendor/
	//./vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/
	./vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/
	./vendor/mediatek/proprietary/hardware/mtkcam/include/
	./vendor/mediatek/proprietary/hardware/mtkcam/utils/fwk/
	./vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor/

	
搜索 KEY_PICTURE_SIZE ：

2、底层相关的文件：编译到的framework和HAL层文件：
frameworks/av/camera/CameraParameters.cpp
frameworks/av/camera/CameraParameters2.cpp
frameworks/av/camera/include/camera/CameraParameters.h
frameworks/base/core/java/android/hardware/Camera.java

hardware/interfaces/camera/common/1.0/default/CameraParameters.cpp
hardware/interfaces/camera/common/1.0/default/include/CameraParameters.h

vendor/mediatek/proprietary/hardware/mtkcam/include/mtkcam/utils/fwk/CameraParameters.h
vendor/mediatek/proprietary/hardware/mtkcam/utils/fwk/CameraParameters.cpp
vendor/mediatek/proprietary/hardware/mtkcam/utils/fwk/MtkCameraParameters.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/params/ParamsManager.update.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/v1/common/paramsmgr/params/ParamsManager.cpp				ParamsManager::getStr
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/MtkEng/MtkEngCamParameter.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/MtkDefault/MtkDefaultCamParameter.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/MtkDefault/MtkDefaultCamAdapter.Capture.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/inc/Scenario/Shot/IShot.h
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/Scenario/Shot/NormalShot/NormalShot.cpp
vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/Scenario/Shot/CapBufShot/CapBufShot.cpp
vendor/mediatek/proprietary/custom/mt6739/hal/sendepfeature/imx190s_mipi_raw/config.ftbl.imx190s_mipi_raw.h


//add for front camera mirror flip start
FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_SUPPORTED(
	KEY_AS_(MtkCameraParameters::KEY_MIRROR_FLIP),
	SCENE_AS_DEFAULT_SCENE(
		ITEM_AS_DEFAULT_(MtkCameraParameters::TRUE),
		ITEM_AS_SUPPORTED_(
			MtkCameraParameters::FALSE
		)
	),
)
//add for front camera mirror flip end


KEY_ANTIBANDING
KEY_VIDEO_SNAPSHOT_SUPPORTED
KEY_ZSD_MODE


74、 sepolicy 

关闭selinux设为Permissive模式：
adb shell setenforce 0


audit2allow的路径在：
audit2allow -i avc_log.txt

[FAQ11483] 如何快速验证 SELinux Policy 问题
MTK 配置目录 alps/device/mediatek/common/sepolicy/  注意的是里面有basic, bsp, full 目录. 
其中basic 目录所有的版本都会吃到; bsp 目录则是bsp 版本 + Turnkey 版本都会吃到;  full 目录则是只有Turnkey 版本会吃到。

/device/mediatek/sepolicy/basic|bsp|full/
non_plat: 仅用于vendor 目录的sepolicy 设定. 和Google vendor 目录类似.
plat_private: 仅用于system 目录的sepolicy 设定, 和Google 的private 目录类似.
plat_public: 同时可用于vendor/system 的sepolicy 设定, 和Google 的public 目录类似.


[FAQ11486] [SELinux Policy] 如何设置SELinux 策略规则 ? 在Kernel Log 中出现"avc: denied" 要如何处理？
[FAQ11483] 如何快速验证 SELinux Policy 问题

N:
     mmm system/sepolicy
     make -j24 ramdisk-nodeps
     make -j24 bootimage-nodeps
 
 然后再重新刷bootimage 测试.
 
O:
     mmm system/sepolicy
    然后再根据对应的sepolicy 是存放在system image, 还是 vendor image 对 system, vendor image 分别打包.


75、查看 camera 信息(HAL1 或者 HAL3.2)
adb shell dumpsys media.camera -v 2


cct导出的参数与编译进去的参数不一样
kernel中宏MAX_NUM_OF_SUPPORT_SENSOR和device中的不一致导致的


camera 18：9 分辨率：
preview 添加："1440x720",
capture添加： "4032x2016",
video 添加："1440x720",
ic为imx214 13m sensor


查看CPU温度：
cat /proc/mtktz/mtktscpu
cat /sys/devices/virtual/thermal/thermal_zone1/temp


camera 打闪阈值修改：

修改 g_AE_PreviewAutoTable  g_AE_CaptureAutoTable   g_AE_VideoAutoTable


camera 差值：
1、计算方法：
（1）4：3的插值公式(n自己调整)：
X = 16*4*n
Y = 16*3*n

（2）16:9的插值
X = 16*16*n
Y = 16*9*n

（3）18:9的插值
X = 16*18*n
Y = 16*9*n

2、为了能够过CTS，要保证最大 KEY_PICTURE_SIZE 和 最大 KEY_PREVIEW_SIZE 的比例一样，都是16:9或者都是4:3.


HAL层的lens代码位置：
hardware/mtkcam/legacy/platform/mt6739/core/featureio/drv/lens
mtkcam/legacy/platform/mt6739/core/featureio/drv/inc/mcu_dev.h


int regulator_is_enabled(struct regulator *regulator)
返回值：
如果regular已经使能了，返回正数
如果regular没有使能，返回0
否则，返回负数 error code。


int regulator_enable/regulator_disable(struct regulator *regulator)
返回值：
0，disable 成功
非0， disable 失败

int regulator_set_voltage(struct regulator *regulator, int min_uV, int max_uV)
返回值：
0，成功设置电压
负数，error code，失败


AF单独供电流程分析：
一、下面是没有修改之前的代码：

1、打开后摄时：
1）首先上三路电压：
[ 1541.338905] <0>.(0)[7820:initCamdevice]ldo vcama enable
[ 1541.342002] <0>.(0)[7820:initCamdevice]ldo vcamio enable
[ 1541.343862] <0>.(0)[7820:initCamdevice]ldo vcamd enable
2）然后AF供电：
2.1）先AF open
[ 1541.581185] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Open] Start
											g_s4AF_Opened = 1;
[ 1541.581859] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1541.583086] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 1	//regular已经使能
[ 1541.584066] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] AF Power on
[ 1541.585064] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Open] End

2.2）然后 AF set motor name
[ 1541.585730] <1>.(1)[7824:3ATHREAD]MAINAF [AF_SetMotorName] Set Motor Name : DW9763AF (6)
											g_pAF_Opened = g_s4AF_Opened = 1;
2.3）然后 AF release
[ 1541.586772] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Release] Start
[ 1541.587470] <1>.(1)[7824:3ATHREAD] DW9763AF_Release Start
[ 1541.588214] <1>.(1)[7824:3ATHREAD] Free 288 
[ 1541.588821] <1>.(1)[7824:3ATHREAD] DW9763AF_Release End
											g_pAF_Opened = g_s4AF_Opened = 0;
[ 1541.589548] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1541.590714] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 1	//regular已经使能
[ 1541.591641] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] Camera Power enable
[ 1541.595164] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_disable 0		//这里会对 regulator_disable()
[ 1541.596083] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1541.597510] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Release] End

2.4）然后 AF open 
[ 1541.603611] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Open] Start
											g_s4AF_Opened = 1;
[ 1541.604284] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1541.605448] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 0	//regular处于 disable 的状态
[ 1541.606376] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_set_voltage 0
[ 1541.608756] <1>.(1)[7824:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_enable 0		//会对 regulator_enable()
[ 1541.615329] <1>.(1)[7824:3ATHREAD]MAINAF [AF_Open] End

2.5）然后 AF set motor name
[ 1541.616003] <1>.(1)[7824:3ATHREAD]MAINAF [AF_SetMotorName] Set Motor Name : DW9763AF (6)
											g_pAF_Opened = g_s4AF_Opened = 1;

2.6）然后初始化 AF（调用 dw9763中的 moveAF函数）
[ 1541.619610] <1>.(1)[7824:3ATHREAD] initdrv success
[ 1541.620562] <3>.(3)[7824:3ATHREAD] Init Pos      0
											g_pAF_Opened = g_s4AF_Opened = 2;

2.7）然后移动AF，实现自动对焦
[ 1542.537557] <3>.(3)[7838:AFthread]moveAF, 204.  move [curr] 327 [target] 343
[ 1542.576100] <0>.(0)[7838:AFthread]moveAF, 204.  move [curr] 343 [target] 357
[ 1542.616521] <3>.(3)[7838:AFthread]moveAF, 204.  move [curr] 357 [target] 371
[ 1542.657741] <0>.(0)[7838:AFthread]moveAF, 204.  move [curr] 371 [target] 387


2、关闭后摄时：

2.1）AF首先下电，调用 AF release 函数
[ 1544.111985] <3>.(3)[504:camerahalserver]MAINAF [AF_Release] Start
[ 1544.112764] <3>.(3)[504:camerahalserver] DW9763AF_Release Start
[ 1544.119070] <3>.(3)[504:camerahalserver] Wait DW9763AF_Release 276 
[ 1544.262137] <2>.(2)[504:camerahalserver] Free 288 
[ 1544.262827] <2>.(2)[504:camerahalserver] DW9763AF_Release End
											g_pAF_Opened = g_s4AF_Opened = 0;
[ 1544.264151] <2>.(2)[504:camerahalserver]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1544.265458] <2>.(2)[504:camerahalserver]MAINAF [AFRegulatorCtrl] regulator_is_enabled 1		//regular已经使能
[ 1544.266462] <2>.(2)[504:camerahalserver]MAINAF [AFRegulatorCtrl] Camera Power enable
[ 1544.267441] <2>.(2)[504:camerahalserver]MAINAF [AFRegulatorCtrl] regulator_disable 0			//这里会对 regulator_disable()
[ 1544.268399] <2>.(2)[504:camerahalserver]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf017480
[ 1544.269626] <2>.(2)[504:camerahalserver]MAINAF [AF_Release] End
[ 1544.321462] <3>.(3)[504:camerahalserver]WARNING: CPU: 3 PID: 504 at /home//work/comio-mtk6739-M9528-W5528-userdebug/kernel-4.4/drivers/regulator/core.c:2200 _regulator_disable+0x38/0x114()
[ 1544.342047] <1>.(1)[504:camerahalserver]unbalanced disables for vldo28


2.2）然后下三路电压
[ 1544.378408] <1>.(1)[504:camerahalserver]ldo vcamd disable
[ 1544.379094] <1>.(1)[504:camerahalserver]ldo vcamio disable
[ 1544.379783] <1>.(1)[504:camerahalserver]ldo vcama disable
[ 1559.886099] <3>.(3)[504:camerahalserver]imgsensor_hw_power_sequence, delay 30



二、下面是修改之后的代码：


1、打开后摄
1.1）使能三路电压
[  112.541577] <2>.(2)[5254:initCamdevice]ldo vcama enable
[  112.542854] <2>.(2)[5254:initCamdevice]lwang.regulator.enable: regulator_set.139. powertype:3. powerId:2800000
[  112.544146] <2>.(2)[5254:initCamdevice]ldo vcamio enable
[  112.545384] <2>.(2)[5254:initCamdevice]lwang.regulator.enable: regulator_set.139. powertype:5. powerId:1800000
[  112.546678] <2>.(2)[5254:initCamdevice]ldo vcamd enable
[  112.547753] <0>.(0)[5254:initCamdevice]lwang.regulator.enable: regulator_set.139. powertype:4. powerId:1000000

[  113.000798] <0>.(0)[5256:3ATHREAD]MAINAF [AF_Open] Start
											g_s4AF_Opened = 1;
[  113.001474] <0>.(0)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  113.002726] <0>.(0)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 0
[  113.003681] <0>.(0)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_set_voltage 0
[  113.006079] <0>.(0)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_enable 0		//regulator_enable
[  113.012535] <1>.(1)[5256:3ATHREAD]MAINAF [AF_Open] End

[  113.013212] <1>.(1)[5256:3ATHREAD]MAINAF [AF_SetMotorName] Set Motor Name : DW9763AF (6)
											g_pAF_Opened = g_s4AF_Opened = 1;

[  113.014231] <1>.(1)[5256:3ATHREAD]MAINAF [AF_Release] Start
[  113.014923] <1>.(1)[5256:3ATHREAD] DW9763AF_Release Start
[  113.015677] <1>.(1)[5256:3ATHREAD] Free 288 
[  113.016329] <1>.(1)[5256:3ATHREAD] DW9763AF_Release End
											g_pAF_Opened = g_s4AF_Opened = 0;

[  113.017062] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  113.018235] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 1
[  113.019159] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] Camera Power enable
[  113.020076] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_disable 0		//regulator_disable
[  113.020973] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  113.022741] <1>.(1)[5256:3ATHREAD]MAINAF [AF_Release] End

[  113.026078] <1>.(1)[5256:3ATHREAD]MAINAF [AF_Open] Start
											g_s4AF_Opened = 1;
[  113.026754] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  113.027919] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_is_enabled 0
[  113.028868] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_set_voltage 0
[  113.031264] <1>.(1)[5256:3ATHREAD]MAINAF [AFRegulatorCtrl] regulator_enable 0		//regulator_enable
[  113.037785] <3>.(3)[5256:3ATHREAD]MAINAF [AF_Open] End

[  113.038462] <3>.(3)[5256:3ATHREAD]MAINAF [AF_SetMotorName] Set Motor Name : DW9763AF (6)
											g_pAF_Opened = g_s4AF_Opened = 1;

[  113.043438] <0>.(0)[5256:3ATHREAD] initdrv success
[  113.044368] <0>.(0)[5256:3ATHREAD] Init Pos      0
											g_pAF_Opened = g_s4AF_Opened = 2;

[  114.033623] <1>.(1)[5266:AFthread]moveAF, 204.  move [curr] 311 [target] 327
[  114.066919] <1>.(1)[5266:AFthread]moveAF, 204.  move [curr] 327 [target] 343
[  114.100447] <1>.(1)[5266:AFthread]moveAF, 204.  move [curr] 343 [target] 357
[  114.134006] <1>.(1)[5266:AFthread]moveAF, 204.  move [curr] 357 [target] 371

[  210.102677] <2>.(2)[2552:HwBinder:465_4]MAINAF [AF_Release] Start
[  210.103486] <2>.(2)[2552:HwBinder:465_4] DW9763AF_Release Start
[  210.104299] <2>.(2)[2552:HwBinder:465_4] Wait DW9763AF_Release 276 
[  210.221923] <1>.(1)[2552:HwBinder:465_4] Free 288 
[  210.222604] <1>.(1)[2552:HwBinder:465_4] DW9763AF_Release End
											g_pAF_Opened = g_s4AF_Opened = 0;
											
[  210.223391] <1>.(1)[2552:HwBinder:465_4]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  210.224625] <1>.(1)[2552:HwBinder:465_4]MAINAF [AFRegulatorCtrl] regulator_is_enabled 1
[  210.225614] <1>.(1)[2552:HwBinder:465_4]MAINAF [AFRegulatorCtrl] Camera Power enable
[  210.226595] <1>.(1)[2552:HwBinder:465_4]MAINAF [AFRegulatorCtrl] regulator_disable 0		//regulator_disable
[  210.227550] <1>.(1)[2552:HwBinder:465_4]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put cf0058c0
[  210.228928] <1>.(1)[2552:HwBinder:465_4]MAINAF [AF_Release] End

[  210.238666] <1>.(1)[2552:HwBinder:465_4]ldo vcamd disable
[  210.239370] <1>.(1)[2552:HwBinder:465_4]lwang.regulator.disable: regulator_set.149. powertype:4. powerId:0
[  210.240576] <1>.(1)[2552:HwBinder:465_4]ldo vcamio disable
[  210.241257] <1>.(1)[2552:HwBinder:465_4]lwang.regulator.disable: regulator_set.149. powertype:5. powerId:0
[  210.242533] <1>.(1)[2552:HwBinder:465_4]ldo vcama disable
[  210.243208] <1>.(1)[2552:HwBinder:465_4]lwang.regulator.disable: regulator_set.149. powertype:3. powerId:0

[  210.274576] <1>.(1)[2552:HwBinder:465_4]imgsensor_hw_power_sequence, delay 30



查看Android是32位还是64位：
cat /proc/cpuinfo 


gitk .
查看：
Follows: INIT-BRANCH-mtk6739-O1-Comio-Trunck, t-alps-release-o1.mp1-V1.102


内核中的定时器：
HZ：表示每秒内核定时器有HZ次时钟中断。
jiffies：每发生一次时钟中断， jiffies 的值加1。在系统引导时，它被初始化为0。它表示当前的中断次数。
jiffies + HZ ： 表示未来的1秒
jiffies + HZ/2 ： 表示未来的0.5秒
jiffies + n*HZ/1000 ： 表示未来的n毫秒


adb shell dumpsys meminfo |grep "camerahalserver"

 PixId:    8,    //otp烧录的first pixel:其中{0,2，4,8} 分别对应 {B,Gb,Gr,R},注意与kernel中的代码不同



怎么删除摄像头的 nvram
android 8.1:
adb shell
su
rm -rf /system/vendor/nvdata
rm -rf /vendor/nvdata

android 7.1 
rm -rf /data/nvram
rm -rf /nvdata


查看属性：
getprop


mtk释放版本查看： vendor/mediatek/release_note/MT6739


#是把宏参数变为一个字符串，##是把两个宏参数连接在一起，注意只能连接两个宏参数
#define A(x) T_##x
#define C(x) #x
A(1)------〉T_1
C(1)------〉"1"
凡宏定义里有用'#'或'##'的地方宏参数是不会再展开。然而解决这个问题的方法很简单. 加多一层中间转换宏. 
加这层宏的用意是把所有宏的参数在这层里全部展开, 那么在转换宏里的那一个宏就能得到正确的宏参数。在转换宏中展开。然后再转换成字符串。
https://blog.csdn.net/qq1321211724/article/details/78561065
#define TOSTRING(value)           #value
#define STRINGIZE(stringizedName) TOSTRING(stringizedName)
char *sensor_kconfig = STRINGIZE(CONFIG_CUSTOM_KERNEL_IMGSENSOR);


例：char *str="abcd";
memset(str+3, 0, 1) ==> str="abc"
memset(str+3,'\0',1) ==> str="abc" 此二者等价

memset(str+3,'0',1) ==> str="abc0"

字符串都已 '\0' 或者 0 结尾，表示一个字符串到了尾部。


Linux邮件列表：
https://lkml.org/lkml/


adb push factory ./system/bin 这样不行
adb push factory /system/bin/factory 这样可以


查看内存
procrank


ProjectConfig.mk中的CUSTOM_HAL_IMGSENSOR配置，只用在了vendor目录下面的Android.mk文件里面。

摄像头模组的构成：
lens镜头、VCM音圈马达、sensor感光芯片、driver IC。


git 提交代码方法：
git add/rm
git commit  -m "test"
git push ssh://@1.2.3.4:29418/xxxx/vendor mtk8321-L1-Pos:refs/for/mtk8321-L1-Pos --tags

git push ssh://@1.2.3.4:29418/xxxx/device mtk8321-L1-Pos:refs/for/mtk8321-L1-Pos --tags

git push ssh://@1.2.3.4:29418/xxxx/docs mtk8321-L1-Pos:refs/for/mtk8321-L1-Pos --tags

查看GPIO状态：
cat /sys/devices/virtual/misc/mtgpio/pin

*(*(a+0)+2);  	// a[0][2] = 3


*(remote_name+1);	//指向remote_name[1][0]

蓝牙连接device流程：
01-04 19:12:58.858  2462  2483 D BluetoothRemoteDevices: aclStateChangeCallback: State:Connected to Device:F4:F5:DB:50:5D:CB

关键log：
btmtk_jsr82_sdp_registration
btmtk_inquired_dev_cache_find found
0xF4:0xF5:0xDB:0x50:0x5D:0xCB

adb push MH1902_sec.bin /system/vendor/firmware/app.bin
adb push app_unsigned.bin /system/vendor/firmware/app.bin

安装NTP到MT8321：直接使用socket实现，注意机器必须联网才能更新NTP时间。
https://blog.csdn.net/u013485792/article/details/51154583


安装arm-none-linux-gnueabi：
wget http://www.codesourcery.com/sgpp/lite/arm/portal/package4571/public/arm-none-linux-gnueabi/arm-2009q1-203-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2
解压后复制到：/usr/local/arm-2009q1
chmod添加权限。
.bashrc中添加：export PATH=/usr/bin:/usr/local/arm-2009q1/bin:$PATH
. ~/.bashrc

arm-none-linux-gnueabi-gcc 和cc1在Linux系统中各自只能有一个，删掉/usr/bin 下面的即可。


./configure --enable-static --prefix=$PWD/install --exec-prefix=$PWD/install --host=arm-none-linux-gnueabi CC=arm-none-linux-gnueabi-gcc
./configure --enable-static --prefix=$PWD/install --exec-prefix=$PWD/install --host=arm-none-linux-gnueabi CC=arm-none-linux-gnueabi-gcc --with-yielding-select=yes
make
make install

将install/bin的文件复制到：
device/mediatek/mt6580/device.mk
PRODUCT_COPY_FILES += vendor/top/topwise/tools/busybox:/system/bin/busybox
PRODUCT_COPY_FILES += vendor/top/topwise/tools/wpa_cli:/system/bin/wpa_cli

在机器终端执行：ntpdate
出错：/system/bin/sh: ntpdate: No such file or directory
busybox可以执行。

@:/work2/ntp-4.2.8p12$ file busybox 
busybox: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV), statically linked, stripped

@:/work2/ntp-4.2.8p12/ntp-4.2.8p12/install/bin$ file ntpdate 
ntpdate: ELF 32-bit LSB  shared object, ARM, EABI5 version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.14, not stripped

@:/work2/ntp-4.2.8p12$ file date
date: ELF 32-bit LSB  shared object, ARM, EABI5 version 1 (SYSV), dynamically linked (uses shared libs), stripped

@:/work2/ntp-4.2.8p12$ file sh
sh: ELF 32-bit LSB  shared object, ARM, EABI5 version 1 (SYSV), dynamically linked (uses shared libs), stripped


./configure -help

./configure --enable-static=yes --enable-shared=no --prefix=$PWD/install --exec-prefix=$PWD/install --host=arm-none-linux-gnueabi CC=arm-none-linux-gnueabi-gcc --with-arch=armv7-a-neon --with-yielding-select=yes
make
make install

--with-gnu-ld=yes
ac_cv_c_bigendian=no

https://blog.csdn.net/qiuen311/article/details/41647637
https://blog.csdn.net/zmc1216/article/details/46777897?locationNum=3
https://bbs.csdn.net/topics/390349057?page=1#post-393513107

readelf -l ntpdate
mount -o rw,remount /
mkdir /lib



"202.112.31.197"
"ntp.neu.edu.cn"


这个一定可以打印出log：
#define PDEBUG(fmt, args...) printf("[%s:%d]" fmt "\n", __func__, __LINE__, ##args)

```


