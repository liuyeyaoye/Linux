

```c

6. atomic_xchg
static inline int atomic_xchg(atomic_t *v, int new);
将新值存入atomic_t类型的变量，并将变量的旧值返回。它使用xchg指令实现。

7. 
b = a<<n,b 等于a乘以2的n次幂。
b = a>>n,b 等于a除以2的n次幂，取商舍余。

8. #if defined (BOOTLOADER)
BOOTLOADER 表示 lk，而不是preloader。

10. 
把之前的旧文件reset 出来后，复制的别的地方，
再把代码pull到最新，
再复制进去，删除哪个错的文件，最后提交
git reset DZX_LCM-Y87113-V01_ILI9881C_50_IVO_MIPI_DZTX.config

13. 在lk的disp_lcm.c中：
if(lcm_drv->compare_id != NULL)
这里是表示lcm_drv中是否定义了 compare_id 这个函数。

if(lcm_drv->compare_id() != 0)
这里是调用lcdm_drv中的compare_id 这个函数。

14. module_init 声明的作用：表明该函数只做模块初始化的作用。在初始化结束之后，内核便会丢弃该函数。
module_init(initialization_function);
初始化函数应当声明成静态的,因为它们不会在特定文件之外可见。
上述定义中的__init标志可能看起来有点陌生，它对内核来讲是一个暗示,表明该函数只是在初始化时使用。
模块加载器在模块加载后会丢掉这个初始化函数,这样可将该函数占用的内存释放出来，以作他用。
__init和__initdata的使用是可选的,虽然有点繁琐，但是很值得使用。
注意，不要在初始化完成后还使用的函数(或者数据结构)上使用这两个标记。
moudle_init的使用是强制性的,这个宏定义会在模块目标代码中增加一个特殊的段,用于说明内核模块初始化函数所在的位置。没有这个定义,你的初始化函数不会被调用。

16. 内存分配
kmalloc() / kzalloc()：所申请的内存虚拟地址和物理地址都是连续的，一般最大 4M
                释放用 kfree()
vmalloc() / vzalloc()：申请大内存，虚拟地址一定连续，物理地址可能不连续
                释放用 vfree()
vmalloc / vzalloc 需要自己的头文件 <linux/vmalloc.h>
申请优先从高端内存（HIGH）分配内存，高端内存没有时再取低端内存

17. prefetch
就是把数据提前存储到cache中，因为在内存中的读写效率比cache中的低，如果都在cache中操作肯定要更快

18. 互斥体
mutex_lock(&g_color_reg_lock);
mutex_unlock(&g_color_reg_lock);
如果有好几个线程可能同时读写这个数据的话就得用互斥锁。


19. 下载对应Android 8.0　release 分支
repo_google init -u git@1.2.3.4:google/platform/manifest -b android-8.0.0_r17
repo_google sync  -j4
repo_google start master --all

编译环境：
sudo add-apt-repository ppa:openjdk/ppa
sudo add-apt-repository ppa:openjdk-r/ppa
sudo apt-get update
sudo apt-get install openjdk-8-jdk
sudo update-alternatives --config java
sudo update-alternatives --config javac

编译：
source build/envsetup.sh
lunch aosp_arm-eng
make -j4 2>&1 | tee build.log

20. MIPI中设置寄存器的某一位的val：
#define MIPITX_OUTREG32(addr, val) \
	{\
		mt_reg_sync_writel(val, addr);\
/*向地址 addr 中写入 val 值*/
	}

#define MIPITX_OUTREGBIT(TYPE, REG, bit, value)  \
	{\
		do {	\
			TYPE r;\
/*定义一个临时 TYPE 类型的变量 r*/
			mt_reg_sync_writel(INREG32(&REG), &r);	  \
/*
INREG32(&REG) 表示读寄存器 REG 的值。
上面表示将 REG 寄存器的值读出来，存入 地址r 中。
*/
			r.bit = value;	  \
/*设置 结构体 r 的 成员 bit 为 val*/
			MIPITX_OUTREG32(&REG, AS_UINT32(&r));	  \
/*将 结构体 r 中的值读出来，写入 REG 中*/
			} while (0);\
	}

MIPITX_OUTREGBIT 的含义是设置寄存器 REG 的成员 bit 的值为 val 。

21. 
#define ALIGN_TO(x, n)    (((x) + ((n) - 1)) & ~((n) - 1))
这个宏的功能是整数提升，即将x提升为n定义的整数的倍数。例如：n=8,x=7，则这条宏运行的结果是8；n=8,x=13，则这条宏运行的结果是16。

22. 
DSI video 的三种模式：同步脉冲模式，同步事件模式，Burst模式，它们的数据传输的格式不一样。
在函数 DSI_PHY_CLK_LP_PerLine_config 可以看到它们的不同之处。

23. dsi video 模式等待忙，在函数 ddp_dsi_stop 中：
	unsigned int tmp = 0;
	while (1) {
		tmp = INREG32(&DSI_REG[i]->DSI_INTSTA);
		if (!(tmp & 0x80000000))
			break;
/*如果忙，循环等待，直到不忙，退出循环*/
	}	

dsi cmd 模式等待忙：
DSI_WaitForNotBusy(module, cmdq_handle);

24. git 使用
使用 git add teset.c 之后，test.c变成 commit 的绿色状态。
然后使用 git reset test.c ，test.c 变成红色(未commit的状态)。
git reset --hard ：重置所有修改。

25. 
struct a {
	int val;
	char b;
};
struct a ta = {0};

struct b {
	unsigned long fixed;
	unsigned int * param[5];
};
struct b tb = {0};

struct a * pa = &ta ;
struct b * pb = &tb;

pa = (struct a *) pb;

指针是指向某一块内存首地址的变量。指针变量的类型，指出了它指向的变量占用的内存大小。所以，指针的强制转换，只能从大转换成小，不能从小转换到大。

例如：
unsigned long *params
unsigned int level = params[0];
unsigned char x = (unsigned char *)&level)

26. 进入 dsi 命令模式前，必须使用 DSI_WaitForNotBusy(module, cmdq); 等待上一个命令执行结束。

27. 在 DSI_dcs_read_lcm_reg_v2 函数中：
t0.Data_ID = (cmd <  0xB0) ? DSI_DCS_READ_PACKET_ID :  DSI_GERNERIC_READ_LONG_PACKET_ID;
DSI_DCS_READ_PACKET_ID：0x06  ,DCS 读 LCM REG，no params。
DSI_GERNERIC_READ_LONG_PACKET_ID：0x14    generic 读 ，1 params。

28. cmdq：将命令放入一个缓冲区里面，等待被 PC 指令执行。
p_command = (uint32_t *) ((uint8_t *) handle->pBuffer + handle->blockSize);
	case CMDQ_CODE_MOVE:
		*p_command++ = 0; /**/
		*p_command++ = CMDQ_CODE_MOVE << 24 | (arg_a & 0xffffff); /**/
		break;

29. 在 ddp_dsi_build_cmdq 函数中，有 state == CMDQ_ESD_CHECK_READ 的处理。

30.
~(u32) 0 	与  ~0 是不同的。
~(u32)0 = 1111 1111 1111 1111
~0  		   = 1

31.
GPR：通用目的寄存器

printk输出的日志信息保存在/proc/kmsg
dmesg 在不刷新缓冲区的情况下获得缓冲区的内容，并将内容返回给stdout。

捕获终端内容：
sudo script kmsg1.log
exit

adb shell dmesg > dmesg.log

cat proc/kmsg |grep -r "aal"
grep -E "aaa|bbb"


<3>[  283.562115]  (0)[53:pmic_thread]kpd: Power Key generate, pressed=1
<4>[  284.048875]  (1)[371:AALServiceMain][PWM] backlight is on (244), ddp_pwm power:(1)<212 -> 5095> 

<3>[  281.072144]  (0)[53:pmic_thread]kpd: Power Key generate, pressed=0
<4>[  282.412606]  (1)[235:surfaceflinger][PWM] backlight is off, ddp_pwm power:(0)



33.
ESD Check
Command mode, Please give priority to the use of pollingTE method
Video mode, Please do ESD by reading the LCM IC register


35.aal
Save the power consumption
Enhance the sun-light read-ability

36.生效 .bashrc
source ~/.bashrc

37. DFO : dynamic display resolution adjustment


38.module_platform_driver(gpio_led_mtk_driver);

module_platform_driver(xxx)；
最终展开后就是如下形式：
static int __init xxx_init(void)
{
	return platform_driver_register(&xxx);
}
module_init(xxx_init);
static void __exit xxx_exit(void)
{
	return platform_driver_unregister(&xxx);
}
module_exit(xxx_exit);


39. MODULE_DEVICE_TABLE(platform, xx_driver_ids);
MODULE_DEVICE_TABLE 一般用在热插拔的设备驱动中。
上述xx_driver_ids结构，是此驱动所支持的设备列表。
作用是：将xx_driver_ids结构输出到用户空间，这样模块加载系统在加载模块时，就知道了什么模块对应什么硬件设备。
用法是：MODULE_DEVICE_TABLE（设备类型，设备表），其中，设备类型，包括USB,PCI等，也可以自己起名字，上述代码中是针对不同的平台分的类；
设备表也是自己定义的，它的最后一项必须是空，用来标识结束。

40，在64位上，指针变量占8个字节。根据gcc和系统位数来确定字节数。

41， mtk的PWM在使用时，才会使能。lcd的 backlight 模式为： MT65XX_LED_MODE_CUST_BLS_PWM
red、green、blue的模式为： MT65XX_LED_MODE_GPIO

42， 在做器件测试(gamma测试等)时，如果gamma测试失败，关闭 mtk aal 支持：
在 defconfig 中 ： #CONFIG_MTK_AAL_SUPPORT=y
在 ProjectConfig.mk 中： MTK_AAL_SUPPORT = no

43， 同一级别的 module_init ，加载顺序为：
同一级别的init，看makefile 加载顺序，obj -y += xxx.o在前面的就先启动。

例如 mtk tp 驱动：

makefile文件为：
obj-y  += tpd_common_probe.o
obj-y  += mstar_drv/
obj-y	+= focaltech_touch_V1_4/

drivers\input\touchscreen\mediatek\tpd_common_probe.c
late_initcall(tpd_probe_init);  /*优先级为7*/

drivers\input\touchscreen\mediatek\mstar_drv\ilitek_drv_mtk.c    这个模块先被执行
module_init(tpd_driver_init);  /*优先级为6*/

drivers\input\touchscreen\mediatek\focaltech_touch_v1_4\focaltech_core.c    这个模块后被执行
module_init(tpd_driver_init);  /*优先级为6*/


46， lcd 时钟
clock-frequency = (h_active + hfp + hbp + h_sync) * (v_active + vfp + vbp + v_sync) * fps



48， video 模式的三种模式：
Non-burst mode sync pluses
Non-burst mode sync event
Burst mode

cat proc/bootprof


adb命令大全：

手机的存储空间在 adb shell 中的路径为：/sdcard/

49. 屏幕截图
adb shell /system/bin/screencap -p /sdcard/screenshot.png 
adb pull /sdcard/screenshot.png ./screenshot.png

50. 从手机中获取 mtklog
adb pull /storage/emulated/0/mtklog mklog/

51.安装/卸载应用程序，-r表示替换原来的 apk
adb install -r 应用程序.apk
adb uninstall xxx.apk

52.发送文件到手机中
adb push test.c /sdcard/test

53.打印 log并输出到文件：查看上层log信息:
 adb logcat |grep "AAL"
 adb logcat > ./log.txt
 adb logcat > ./camera.txt

adb 录制屏幕：
adb shell 
screenrecord /sdcard/demo.mp4	//180s

adb shell screenrecord  --time-limit 10 /sdcard/demo.mp4	//10s

adb shell screenrecord --size 1280*720 /sdcard/demo.mp4

adb pull /sdcard/demo.mp4



54.查看内核信息
sudo script kmsg.log
exit
cat /proc/kmsg

55.adb 进入工程模式
adb shell am start -n com.mediatek.engineermode/.EngineerMode

adb 亮屏：
adb shell input keyevent 26

56.如何控制振动器
adb shell
echo 3000 > /sys/devices/virtual/timed_output/vibrator/enable
震动3秒


system/core/rootdir/init.rc
device/mediatek/mt6735/init.mt6735.rc
out/target/product/gerneal/root/init.mt6735.rc


59、 编写 dts 文件注意：
内容离顶端1个或者2 个tab 键,（from M_branch_OTP_porting_guide.pdf   page 8）

&i2c0 {
	#address-cells = <1>;
	#size-cells = <0>;
	camera_main@10 {
		compatible = "mediatek,camera_main";
		reg = <0x10>;
		status = "okay";
	};
	camera_main_af@0c {
		compatible = "mediatek,camera_main_af";
		reg = <0x0c>;
		status = "okay";
	};
	camera_sub@3c {
		compatible = "mediatek,camera_sub";
		reg = <0x3c>;
		status = "okay";
	};
};



60、
#define A1(name, type)  type name_##type##_type 
#define A2(name, type)  type name##_##type##_type

A1(a1, int);  	/* 等价于: int name_int_type; */
A2(a1, int);  	/* 等价于: int a1_int_type;   */



61、 camera bring up 
(1)
D1使用 mt6737t 
D2 使用 mt6737m 	mt6735m


MT6737V/W					MACH_TYPE=mt6737m
MT6737V/WT				MACH_TYPE=mt6737t
MT6737V/WH   			MACH_TYPE=mt6737t

ov8856b			后摄
OV9760    		后摄
ov8856			后摄
OV9760s 		前摄

如果一个 层camera ，有2种，后缀为 b 的为后摄，后缀为 s 的为前摄。

夹具：
一般前摄 gc0310_mipi_yuv 
后摄 gc2155_mipi_yuv 或 gc2355_mipi_raw


62.固件升级和格式化下载的区别：
下载不会格式化。
格式化是代表整个内存的格式化。
固件升级只会格式化系统和用户的2个部分而已，不是整个内存。


sudo apt-get add-apt-repository ppa:git-core/ppa
sudo apt-get update
sudo apt-get install git


67、strlen函数是不包括‘\0’的长度的，sizeof计算的结果才包括'\0'的长度。当要取一个字符串的长度时候，使用strlen.


9、GPIO配置可以这样读出来：
adb shell cat /sys/devices/platform/1000b000.pinctrl/mt_gpio

 [MODE] [DIR] [DOUT] [DIN] [PULL_EN] [PULL_SEL] [IES] [SMT] [DRIVE]
  95: 110010101
  96: 110010101
 112: 110010101


问题：在defconfig中有：CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_FT8006M=y    在 .config 中却没有这个配置；
解决：在 proj.cfg 中将 CONFIG_TOUCHSCREEN_MTK_FOCALTECH_TS_FT8006M=y 提前。也无效，直接在私有化文件中：obj-y   +=  ...
原来是没有配置 Kconfig文件。漏编译 tpd_common_probe.o 

问题：tp可以划动，但是相反了。
解决：修改 report data 代码。



编译报错：回退版本：
cd idh.code/packages/apps/360OS_Launcher
gitk .
git reset --hard f337c9760dfd7705d87a75f776bcf59789c3daf1
序号为 commit ID ，不是 change ID 。


10、git使用方法：
(1)如果发生冲突，在 Gerrit 上点击 abandon ，然后在本地 git  reset --hard 		git pull
(2)在本地回退到某个版本 cd dir		git reset --hard commitID

(3)在提交时候，将 yes 写成了 yesa ，导致在本地分支提交了，但是在远程分支却没有。
先把提交的文件剪切到别的目录，然后git rm 删掉这个提交的文件，然后pushcode ，将文件提交：此时在Gerrit上，会出现2次提交，abandon那次无效的提交即可。


11、mt6737t  的头文件为 mt6735.dtsi 和 cust.dtsi
mt6737m  的头文件为 mt6735m.dtsi 

12、i2c设备驱动成员的 .detect 和 .address_data 可以省略，id_table 不能省略。（kxtj_1009.c 中）

13、kxtj_1009.c 中， platform_driver 和 kxtj2_1009_i2c_driver 公用同一个节点： .compatible = "mediatek,gsensor"

14、HAL 层用到的所有头文件在这个目录：
device/mediatek/common/kernel-headers/

15、 camera 3A算法：AE/AF/AWB 。对应的是相机的功能，即自动曝光，自动对焦和自动白平衡

16、incell 屏。

17、如果下载不进去时，使用 全部格式化和下载即可。

18、烧机时提示： ERROR： S_INVALID_BBCHIP_TYPE
解决：CPU型号不对。

20、当不能刷机时，USB显示为：Mediatek Inc 。手动选择USB即可刷机。

21、刷机时，txt文件和要下载的img文件，必须在同一个目录才能下载。

22、在 编译 kernel 时，注意要先删除 out 目录下上次编译产生的相关的目录和 .o  文件。

23、 Linux V4L2 是针对 UVC(usb视频规范)规范的，即USB摄像头。UVC 驱动路径为： kernel-3.10/drivers/media/usb/uvc/
MTK标准的Camera并没有采用v4l2框架，所以需要在jni层实现基本的v4l2视频采集流程。

27、 camera驱动：HAL层的sensorlist.cpp 和 kernel层的 kd_sensorlist.h ，顺序保存一致。

28、Ubuntu没有声音，配置成模拟输出。

29、 Makefile
$(wildcard PATTERN...) 。在Makefile中，它被展开为已经存在的、使用空格分开的、匹配此模式的所有文件列表。
如果不存在任何符合此模式的文件，函数会忽略模式字符并返回空。
例：
$(	wildcard 	$(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/$(1)	)


31.
source build/envsetup.sh
lunch
mmm vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6735/v1/common/paramsmgr

32.
git checkout -f
放弃对文件的修改

git reset --hard commitID
可以回退某次commitID提交commit。

git reset --hard
彻底回退到某个版本，本地的源码也会变为上一个版本的内容

33、函数指针就是函数名，调用一个函数指针，相当于调用这个函数名。


34、Ubuntu怎么连接到服务器：
在Ubuntu中的文件中，点击连接到服务器，然后输入： smb://data1

git checkout -f 
git clean -fd


38、 MTK i2c 使用7位寻址。当使用 i2c DMA传输数据时，最大传输 255 个字节。

struct i2c_client {
	unsigned short flags;		/* div., see below		*/
	unsigned short addr;		/* chip address - NOTE: 7bit	*/
					/* addresses are stored in the	*/
					/* _LOWER_ 7 bits		*/
	char name[I2C_NAME_SIZE];
	struct i2c_adapter *adapter;	/* the adapter we sit on	*/
	struct device dev;		/* the device structure		*/
	int irq;			/* irq issued by device		*/
	struct list_head detected;
#ifdef CONFIG_MTK_I2C_EXTENSION
	__u32 timing;			/* parameters of timings		*/
	__u32 ext_flag;
#endif
};




39、EXPORT_SYMBOL使用方法
   第一、在模块函数定义之后使用EXPORT_SYMBOL（函数名）
   第二、在掉用该函数的模块中使用extern对之声明
   第三、首先加载定义该函数的模块，再加载调用该函数的模块



 要查看log信息 还需要另一个条件 那就是在代码中将usb接口的log信息输出的关闭状态给消除掉
如下：将kernel/printk/printk.c的函数 mt_disable_uart(void) 以及 module_param_named(disable_uart, printk_disable_uart, bool, S_IRUGO | S_IWUSR); 
屏蔽掉 如果不屏蔽掉 就不会打印kernel的log信息
或
使用UE这样的二进制工具打开lk.bin 搜索按ASCLL条件将disable_uart 和printk_disable_uart由默认的1改为0
在下载进板子的时候选择的平台会出现Checksum.ini的报错 这时可以删掉Checksum.ini(这个是校验用的东西 目的是防止你去随意更改代码)
  
40、 LCD的物理尺寸
如果LCD为 5.0 寸，1寸为 25.4 mm， 即 LCD的对角线长度为 12.7 cm 。 屏幕的分辨率为 480*854 ，为 16:9 的屏。根据勾股定律，算出 ：
physic_width  =  62 mm
physic_height =  111 mm
或者直接问项目跟供应商要屏的物理尺寸。

41、 函数中可以传入未初始化的指针，例如下列代码不会出错：

unsigned int a = 6; 
void fun(unsigned int ** liu)
{
	 *liu = & a;
}
int main(void)
{
	 unsigned int * str;	//str 是一个 unsigned int * 类型，但是它没有初始化，而是在函数 fun 中进行初始化。经过 fun 之后， str 指向了全局变量 a 。

	 fun(&str);	// &str 不为 NULL 。

	 printf("%d \n",*str);    //* str = 6

	 return 0;
}


git提交代码
git add .
git commit -m description
git push ssh://xxxx@1.2.3.4:29418/xxxxxL1/kernel-3.10 mtk8321-L1-driver:refs/for/mtk8321-L1-driver --tags   
git push <远程主机名> <本地分支名>:<远程主机分支名>

for mt8321 vendor:   
git push ssh://xxxx@1.2.3.4:29418/xxxxxL1/vendor mtk8321-L1-Pos:refs/for/mtk8321-L1-Pos --tags  
   

lk/app/mt_boot/rules.mk
修改 ：	printk.disable_uart = 0
作用：	user和userdebug版本都有kernel log


MTK平台dump pass1和pass2的数据：

ISP 2.x

Select dump buffer port
Normal Preview, Video Preview/Record and ZSD Preview (using bitmask)
dump Pass1 IMGO output
adb shell setprop camera.dumpbuffer.enable 1
dump Pass2 WDMA output (For Display)
adb shell setprop camera.dumpbuffer.enable 2
dump Pass2 WROT output (For Record)
adb shell setprop camera.dumpbuffer.enable 4
dump Pass1 IMGO and Pass2 WDMA output
adb shell setprop camera.dumpbuffer.enable 3
dump Pass1 IMGO, Pass2 WDMA and  Pass2 WROT output
adb shell setprop camera.dumpbuffer.enable 7
dump Pass1 pass2 when take picture (single shot)
adb shell setprop debug.camera.dump 1
dump VSS Pass1 Pass2
adb shell setprop camera.vss.dumpbuffer.enable 1

Pull buffer from device
adb pull /sdcard/camera_dump/

Clear buffer of device
adb shell rm -rf /sdcard/camera_dump/



ISP 3.x

Jade has display orientation for some features, when you want to dump display or record buffer, please try both WDMA and WROT port.

Select camera mode
Normal Preview
adb shell "setprop debug.PHO.dump 1"
Normal Capture
adb shell "setprop debug.SShot.dump 1"
Continue Shot
adb shell "setprop debug.MShot.dump 1"
Video Preview/Record
adb shell "setprop debug.VDO.dump 1"
ZSD Preview
adb shell "setprop debug.ZSD.dump 1"

Select dump buffer port
Normal Preview, Video Preview/Record and ZSD Preview
dump Pass1 RRZO output
adb shell "setprop debug.camnode.dump.1 1"
dump Pass1 IMGO output
adb shell "setprop debug.camnode.dump.2 1"
dump Pass2 input from RRZO
adb shell "setprop debug.camnode.dump.31 1"
dump Pass2 WDMA output (For Display)
adb shell "setprop debug.camnode.dump.41 1"
dump Pass2 WROT output (For Record)
adb shell "setprop debug.camnode.dump.43 1"
dump Pass2 IMG2O output
adb shell "setprop debug.camnode.dump.45 1"

Normal capture and ZSD capture
dump Pass2 input from RRZO (normal capture only)
adb shell "setprop debug.camnode.dump.31 1"
dump Pass2 input from IMGO
adb shell "setprop debug.camnode.dump.32 1" 
dump Pass2 WDMA output
adb shell "setprop debug.camnode.dump.42 1"
dump Pass2 WROT output
adb shell "setprop debug.camnode.dump.44 1"

Pull buffer from device
Normal Preview
adb pull /sdcard/cameradump_PHO ./cameradump_PHO/
Normal Capture
adb pull /sdcard/cameradump_SShot ./cameradump_SShot/
Continue Shot
adb pull /sdcard/cameradump_SShot ./cameradump_MShot/
Video Preview/Record
adb pull /sdcard/cameradump_VDO ./cameradump_VDO/
ZSD Preview
adb pull /sdcard/cameradump_ZSD ./cameradump_ZSD/

Clear buffer of device
adb shell "rm -rf /sdcard/cameradump_*"



#undef TAG
#define TAG "[USDK] "

ps |grep factory
getevent -l
getevent -t /dev/input/event0

tag:USDK text:TW_Kbd
text:fatal

. ~/.bashrc

```



