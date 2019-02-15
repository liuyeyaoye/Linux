

```c

MTK平台遇到过的问题记录：

一、Z705平板项目
工作：调试前摄GC2385，后摄GC5025
平台：MT8735
安卓版本：Android 8.1
内核版本：kernel-3.18

1、
问题：在点亮 camera 时，有时候修改了代码，编译不能生效，可以在代码中加入一些字符串如 test 来判断是否编译到了这个文件。
解决办法：在修改代码后，touch 一下这个文件，更新文件的时间为系统当前时间，就可以被编译到了。

2、
问题：在点亮 camera 时，发现在 sensorlist.cpp 中添加了这2颗 imgsensor，但是在开机 log 中却没有去 search 这2颗 imgsensor。
原因：在 mt6735 和  tb8735ba1_bsp 这2个目录都有 sensorlist.cpp 这个文件， tb8735ba1_bsp 这个目录下的文件优先级高，修改 mt6735 目录下的这个文件不会有效果。

3、
问题：前摄 GC2385 不能点亮，前摄代码中读出来的ID是 0x5025 ，这是后摄的ID。
原因：前后摄的I2C地址都是0x6e。并且查看硬件原理图可以知道，前后摄的VCAMA、VCAMD、VCAMIO、MCLK都是共用的。在给前摄上电时，由于电压引脚共用并且正好符合后摄上电时序，所以此时后摄也处于工作状态，没有读到前摄的ID，但是读到了后摄的ID，前后摄互相干扰。
解决：修改上电时序，以不让2颗imgsensor相互干扰。
在后摄的上电时序之后，拉低前摄的 PWDN（GC2385s没有RESET引脚，不用管它），让前摄待机。GC2385的datasheet中有：
If the sensor’s power cannot be cut off, please keep power supply, then set PWDN pin low. It will make sensor standby。
在前摄的上电时序之后，拉高后摄的 PWDN，让后摄待机。GC5025的datasheet中有：
If the sensor’s power cannot be cut off, please keep power supply, then set PWDN pin high.It will make sensor standby。
总结：以后出现前后摄共用电压引脚和MCLK引脚，并且二者的i2c地址都是一样的时候，记得处理二者的干扰。


4、
问题：后摄翻转了180度，修改了 tb8735ba1_bsp 目录的 cfg_setting_imgsensor.cpp 文件中的 sensor 安装角度，但是没有生效。
原因：mt8735使用的是HAL3，使用的是metadata，必须修改 metadata才有效。
而在 mt6735 目录有 metadata 的目录，但是在 tb8735ba1_bsp 目录没有 metadata 目录，所以不能生效。
解决：在 tb8735ba1_bsp 新建一个 metadata目录，添加 gc5025 的 metadata代码，修改相关文件中的 sensor orientation ，即可生效。



5、问题：前摄GC2385s读不到ID
分析： imgsensor的配置都正确，上下电时序都是按照数据手册来的。看了开机log，发现错误log：IIC ACKERR。
量了开机时的三路电压和IIC波形，都有，而且都是按照时序来的。
另外，使用此项目使用过的另外一颗前摄模组 sp0a09 ，却可以正常打开。后来查datasheet才知道它只需要2路电压，不需要VCAMD供电即可工作。
原因：后来查看硬件，发现VCAMD与sensor引脚之间的电阻没有接上，导致前摄没有供电。
量波形时的引线接出来的都是引出脚，实际上前摄的 DVDD没有供电。由于开机时候上电时间很短，用万用表测量不到电压。
总结：出现 IIC ACKERR时，如果软件没问题，需要检查主板上有没有缺少某些料。
VCAMA			2.8V
VCAM_IO		1.8V
VCAMD			1.2V
注意：每一颗模组都是不同的，同一型号的模组都有差异，不同型号的模组更加有差异，不能拿在一起比较。
不能说：这颗模组可以点亮，但是那颗模组不可以点亮，那么硬件肯定没问题。



二、W5528手机项目
工作：后摄s5k3l8 的VCM（Voice Coil Motor） driverIC dw9718af 调试
平台：MT6739
安卓版本：Android 8.1
内核版本：kernel-4.4

1、
问题：在点亮 W5528 项目的 S5K3L8 的 DW9718AF时，出现了i2c错误： I2C ACKERR 。
原因：这份 dw9718af 驱动是从 MT6737 平台移植过来的，将它换成供应商提供的 DW9718AF 代码之后(注释掉了 init_drv()这个函数)即可。

2、
问题：在log中看到可以move AF，但是实际上对着书本文字预览的时候却没有效果。
原因：使用胶带粘住了后摄，导致无法移动lens。将胶带去掉后，lens可以移动，可以AF，有效果。

3、W5528开机AF漏电：一开机AF就有电压。
原因：AF与TP共用2.8V电压，TP驱动初始化会把它上电，造成AF漏电。这个TP不需要次电压，所以注释掉TP的2.8V上电。


三、FX707华强北项目LCD调试的VSP和VSN问题
平台：MT6737
安卓版本：Android 7.1
内核版本：kernel-3.18


1、
问题：在调试FX707的LCD时，用万用表测试发现无法将LCD的VSP和VSN电压设置到5.8V（本来是5.4V，需要升到5.8V来解决TP跳点问题）。在lk和kernel中修改，均没有生效。
原因：查看log发现有IIC ACKERR，电压芯片没有工作。
解决：硬件原因，芯片的I2C引脚没有加电阻，所以出现错误。
让硬件加上电压芯片的SCL和SDA的电阻，然后在lk和kernel中添加设置电压的代码之后，用万用表可以测得VSP和VSN升到了5.8V和-5.8V。
注意：IIC ACKERR错误，先查看硬件有没有少贴电阻或电容。



四、W5528手机项目有一台机器出现按power键LCD无法唤醒的问题
1、
问题：按power键，LCD无法唤醒。多按几次，LCD才能唤醒。
分析：查看log，发现每对应一次按下或者松开的log，代码中都有LCD的亮或灭。


01-02 19:31:14.218012    56    56 I [ 9823.943521].(1)[56:pmic_thread]: [PMIC] [PMIC_INT] Reg[0x91a]=0x1
01-02 19:31:14.218947    56    56 D [ 9823.944456].(1)[56:pmic_thread]: (pressed) HW keycode powerkey
01-02 19:31:14.367901    56    56 I [ 9824.093410].(0)[56:pmic_thread]: [PMIC] [PMIC_INT] Reg[0x91a]=0x4
01-02 19:31:14.683450     0     0 W [ 9824.408959].(2)[357:light@2.0-servi]: [PWM] disp_pwm_set_backlight_cmdq(id = 0x1, level_1024 = 0), old = 569

01-02 20:00:46.521538    56    56 I [10057.129681].(0)[56:pmic_thread]: [PMIC] [PMIC_INT] Reg[0x91a]=0x4
01-02 20:00:46.866407   463   463 W [10057.474550].(1)[463:AALServiceMain]: [PWM] disp_pwm_set_backlight_cmdq(id = 0x1, level_1024 = 417), old = 0


解决：因为是单机问题，log中没有错误，怀疑按键损坏，重新更换一个按键后不会出现该问题。



五、W5528手机项目
工作：二供前摄GC5025卡死
平台：MT6739
安卓版本：Android 8.1
内核版本：kernel-4.4

现象：能够读到GC5025的sensor ID 和 OTP 的 module ID，也可以单独点亮，可以和后摄s5k3l8一起点亮，但是不能和后摄imx214一起点亮。
log中出现：  [ISP]v1 ERR WaitIrq Timeout ，pass1][ISP]Fail ，没有收到 pass1 的数据。

初步解决：将gc5025的MCLK从24改成12后，解决。

根本原因：
regulator vcamd 设置电压检查出错，log中可以看到：


[  109.050292] <1>.(1)[6667:initCamdevice]ldo vcamio enable
[  109.052417] <1>.(1)[6667:initCamdevice]vcamd: Restricting voltage, 1200000-1000000uV
[  109.053398] <1>.(1)[6667:initCamdevice][regulator]fail to regulator_set_voltage, powertype:4 powerId:1200000
[  109.055419] <1>.(1)[6667:initCamdevice]ldo vcamd enable
[  109.064812] <1>.(1)[6667:initCamdevice]ldo vcama enable


间接解决：
将后摄 IMX214 的 vcamd 电压从1.0V改成1.2V后，解决：{DVDD, Vol_1000, 0} ——> {DVDD, Vol_1200, 0}
前摄gc5025的 VCAMD 也是 1.2V。

最终解决：
[FAQ20676] camera sensor不同电压的引脚共用regulator的处理方法
对于共用regulator ，但电压需求不同的引脚：regulator 为了保护低电压引脚不被被烧坏，
会把低电压引脚的电压设置成regulator的最高电压，从而使高电压引脚无法上电。解决办法：
(1) 下电后，调用 regulator_put 函数，释放掉 regulator。
(2) 上电前，调用 regulator_get 函数，重新申请 regulator。


该问题续：
1、按照上面的regulator修改还是会出现错误：


06-29 17:43:33.650254  2277  2277 E [   43.838347].(1)[2277:initCamdevice]vcama: unsupportable voltage range: 2900000-2800000uV
06-29 17:43:33.651334  2277  2277 E [   43.839427].(1)[2277:initCamdevice]: [regulator]fail to regulator_set_voltage, powertype:3 powerId:2900000


2、原因
PMIC mt6357 的代码中，对vcama的支持没有2.9V。

3、解决：手动微调pmic电压，可参考pmic datasheet和pmic API实现：


  + #include <mt-plat/upmu_common.h>
  static enum IMGSENSOR_RETURN regulator_set(
  ...
  pregulator = preg->pregulator[reg_type_offset + pin -  IMGSENSOR_HW_PIN_AVDD];
  enable_cnt = preg->enable_cnt + (reg_type_offset + pin - IMGSENSOR_HW_PIN_AVDD);
  + if (pin == IMGSENSOR_HW_PIN_AVDD && pin_state == IMGSENSOR_HW_PIN_STATE_LEVEL_2900) {
      + pin_state == IMGSENSOR_HW_PIN_STATE_LEVEL_2800;  //设定基准电压2.8V
      + pmic_config_interface(0x1C8C, 10, 0xf, 0);   //设定微调电压100mV
  + }
  if (pregulator) {
  if (pin_state != IMGSENSOR_HW_PIN_STATE_LEVEL_0) {
  


六、W5528手机项目迈瑞微指纹驱动，log中无法 set_prop


解决：抓boot kernel log，grep "avc" 

[   14.570541] <1>.(1)[1:init]selinux: avc:  denied  { set } for property=sys.fingerprint.chip pid=509 uid=1000 gid=1000 scontext=u:r:hal_fingerprint_default:s0 tcontext=u:object_r:system_prop:s0 tclass=property_service permissive=0

然后使用audit2allow： sudo audit2allow -i avc.log

会在终端输出：
allow hal_fingerprint_default system_prop:property_service set;

如果提示： unable to open (null):  错误的地址
将avc.txt中的无效行删除即可。

audit2allow 安装方法：
sudo apt-get install policycoreutils
whitch audit2allow



七、W5528手机项目添加前摄imx190/gc5025镜像拍照功能
平台：MT6739
安卓版本：Android 8.1
内核版本：kernel-4.4

参考FAQ实现，在ftbl中添加一个镜像参数，HAL层读取APP设置的这个参数，然后进行处理，涉及到的修改有：


1、framework层：
frameworks/av/camera/CameraParameters.cpp
const char CameraParameters::KEY_MIRROR_FLIP[] = "camera-mirror-flip";

frameworks/av/camera/include/camera/CameraParameters.h
static const char KEY_MIRROR_FLIP[];

2、hardware层：
hardware/interfaces/camera/common/1.0/default/CameraParameters.cpp
const char CameraParameters::KEY_MIRROR_FLIP[] = "camera-mirror-flip";

hardware/interfaces/camera/common/1.0/default/include/CameraParameters.h
static const char KEY_MIRROR_FLIP[];

3、mhal层：
vendor/mediatek/proprietary/hardware/mtkcam/utils/fwk/CameraParameters.cpp
const char CameraParameters::KEY_MIRROR_FLIP[] = "camera-mirror-flip";

vendor/mediatek/proprietary/hardware/mtkcam/include/mtkcam/utils/fwk/CameraParameters.h
static const char KEY_MIRROR_FLIP[];

vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/MtkDefault/MtkDefaultCamAdapter.Capture.cpp
#include <string.h>
#include <utils/String8.h>
String8 s8false("false"), s8flip("false");
s8flip = mpParamsMgr->getStr(CameraParameters::KEY_MIRROR_FLIP);
if ( s8flip ==  s8false) {
	shotParam.flip = MFALSE;
	MY_LOGE("shotParam.flip is MFALSE");
}
else {
	shotParam.flip = MTRUE;
	MY_LOGE("shotParam.flip is MTRUE");
}

	if(getOpenId() == 1 && (shotParam.flip)){
		switch(shotParam.mu4Transform){
			case eTransform_ROT_90:
			shotParam.mu4Transform = eTransform_FLIP_V | eTransform_ROT_90;
			break;
			case eTransform_ROT_270:
			shotParam.mu4Transform = eTransform_FLIP_H | eTransform_ROT_90;
			break;
			case eTransform_ROT_180:
			shotParam.mu4Transform = eTransform_FLIP_V;
			break;
			default:
			shotParam.mu4Transform = eTransform_FLIP_H;
			break;
		}
	}

vendor/mediatek/proprietary/hardware/mtkcam/legacy/platform/mt6739/v1/adapter/inc/Scenario/Shot/IShot.h
, flip(MFALSE)

vendor/mediatek/proprietary/custom/mt6739/hal/sendepfeature/imx190s_mipi_raw/config.ftbl.imx190s_mipi_raw.h
FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_SUPPORTED(
	KEY_AS_(MtkCameraParameters::KEY_MIRROR_FLIP),
	SCENE_AS_DEFAULT_SCENE(
		ITEM_AS_DEFAULT_(MtkCameraParameters::TRUE),
		ITEM_AS_SUPPORTED_(
			MtkCameraParameters::FALSE
		)
	),
)

vendor/mediatek/proprietary/custom/mt6739/hal/sendepfeature/gc5025s_mipi_raw/config.ftbl.gc5025smipi_raw.h
FTABLE_CONFIG_AS_TYPE_OF_DEFAULT_SUPPORTED(
	KEY_AS_(MtkCameraParameters::KEY_MIRROR_FLIP),
	SCENE_AS_DEFAULT_SCENE(
		ITEM_AS_DEFAULT_(MtkCameraParameters::TRUE),
		ITEM_AS_SUPPORTED_(
			MtkCameraParameters::FALSE
		)
	),
)



八、W5528项目迈瑞微/信炜指纹库兼容
不能修改hardware层的hardware.c文件，Android8.1过VTS刷GSI包会替换 system.img 。
要使用HAL层指纹库的module ID来兼容，让FAE修改指纹库的module ID才行。
代码中体现为:


hardware/interfaces/biometrics/fingerprint/2.1/default/BiometricsFingerprint.cpp

hw_get_module(fingerprint_id, &hw_mdl)
自己根据不同的指纹ID客制化 fingerprint_id 即可。


九、代码编译报错


You have tried to change the API from what has been previously approved.
To make these errors go away, you have two choices:
   1) You can add "@hide" javadoc comments to the methods, etc. listed in the errors above.
   2) You can update current.xml by executing the following commands:
         p4 edit frameworks/base/api/current.xml
         make update-api
      To check in the revised current.xml, you will need OWNERS approval


解决： make update-api 或者 添加 /** {@hide} */


十、 W6002手机项目后摄IMX214 imgsensor外挂的EEPROM芯片 iic不通
平台：MT6761
安卓版本：Android8.1
内核版本：kernel-4.9
工作：前摄imx190，后摄imx214调试、AF驱动移植、otp移植：后摄的平台端otp LSC代码移植。

在开机search imgsensor的时候，去读取存储在EEPROM中的otp数据，但是出现了 IIC ACKERR：


[   12.151837] <3>.(1)[507:camerahalserver]get_imgsensor_id, 1968.  i2c write id: 0x20, sensor id: 0x214
[   12.151846] <3>.(1)[507:camerahalserver]imx214_EEPROM_FMT[imx214_read_eeprom_mtk_fmt] OTP readed 0 
[   12.156157] <3>.(1)[507:camerahalserver]i2c i2c-2: error:addr=0x50,irq_stat=0x3,ch_offset=0x100,mask:0x129
[   12.156167] <3>.(1)[507:camerahalserver]i2c i2c-2: addr:0x50,ext_data skip more log
[   12.156170] <3>.(1)[507:camerahalserver]i2c i2c-2: addr:0x50,ACK error
[   12.156183] <3>.(1)[507:camerahalserver][bxx-err]reg, dir:0xffff,DIN:0xffff,rsel:0xff,pu:0x0,pd;0xfffc
[   12.156181] <3>.(1)[507:camerahalserver][bxx-err]reg, 103-mode:0x0,104-mode:0x0,EH:0xb6d
[   12.156185] <3>.(1)[507:camerahalserver]i2c i2c-2: trans done with error
[   12.156207] <3>.(1)[507:camerahalserver]I2C read failed (0x0)! speed(0=400) (0x0)
[   12.156211] <3>.(1)[507:camerahalserver],imx214_read_eeprom_mtk_fmt,169.read imx214_eeprom  i2c fail !!!
[   12.156215] <3>.(1)[507:camerahalserver]get_imgsensor_id,1970. imx214_read_eeprom_mtk_fmt err !!!


在otp烧录规范中有： 该烧录介质为 EEPROM，大小为8K Bytes，可重复烧录。

解决办法：在 regulator 中添加 AF的上电：


	if (pin > IMGSENSOR_HW_PIN_AFVDD   ||
		pin < IMGSENSOR_HW_PIN_AVDD    ||
		pin_state < IMGSENSOR_HW_PIN_STATE_LEVEL_0 ||
		pin_state >= IMGSENSOR_HW_PIN_STATE_LEVEL_HIGH)
		return IMGSENSOR_RETURN_ERROR;


修改将AF的供电跟三路电压一起上下电（这个平台的camera代码中AF的供电单独在AF驱动中实现的），因为有的imgsensor外挂的EEPROM芯片需要AF电压才能工作。
		

注意：


struct REGULATOR_CTRL regulator_control[REGULATOR_TYPE_MAX_NUM] = {
	{"vcama"},
	{"vcamd"},
	{"vcamio"},
	{"vcamaf"},
	{"vcama_sub"},
	{"vcamd_sub"},
	{"vcamio_sub"},
	{"vcama_main2"},
	{"vcamd_main2"},
	{"vcamio_main2"},
	{"vcama_sub2"},
	{"vcamd_sub2"},
	{"vcamio_sub2"}
};


没有用到的 regulator 不能设为 NULL ， 否则机器会重启。找不到的话会使用 dummy regulator：


[    1.119667] <3>.(3)[1:swapper/0]kd_camera_hw supply vcama_main2 not found, using dummy regulator
[    1.120837] <3>.(3)[1:swapper/0]kd_camera_hw supply vcamd_main2 not found, using dummy regulator
[    1.121976] <3>.(3)[1:swapper/0]kd_camera_hw supply vcamio_main2 not found, using dummy regulator
[    1.123126] <3>.(3)[1:swapper/0]kd_camera_hw supply vcama_sub2 not found, using dummy regulator
[    1.124264] <3>.(3)[1:swapper/0]kd_camera_hw supply vcamd_sub2 not found, using dummy regulator
[    1.125396] <3>.(3)[1:swapper/0]kd_camera_hw supply vcamio_sub2 not found, using dummy regulator



十一、 W6001手机项目前摄GC5025点不亮
平台：MT6761
安卓版本：Android8.1
内核版本：kernel-4.9

单点GC5025，进去之后预览黑屏。


[   62.333137] [ISP]v1 ERR WaitIrq Timeout Clear(0), Type(0), IrqStatus(0x00000000), WaitStatus(0x00000001), Timeout(503),user(5)
[   62.845087] [ISP]v1 ERR WaitIrq Timeout Clear(0), Type(0), IrqStatus(0x00000000), WaitStatus(0x00000400), Timeout(501),user(5)
[   62.877084] <1>-(1)[2855:pass1][ISP]v3 ERRRR WaitIrq Timeout(1000) Clear(1), Type(0), IrqStatus(0x00000000), WaitStatus(0x00000400), Timeout(1000),userKey(0)


重新合入gc提供的mt6763平台的gc5025代码后，解决。



十二、 W6001手机项目 k61v1_64_bsp_debug_defconfig 不生效问题


CONFIG_MTK_ACCDET=y
CONFIG_ACCDET_EINT_IRQ=y
+CONFIG_ACCDET_SUPPORT_EINT0=y
-# CONFIG_ACCDET_EINT_IRQ is not set
-# CONFIG_ACCDET_SUPPORT_EINT0 is not set


在添加一个config之后，需要把 XXX is not set 删掉，该config才会生效。



十三、 W6001手机项目 imx214otp.c 编译老是报错:  64 bit kernel兼容不对
修改成下面后ok：


#if 1
    err |= get_user(p, data->pu1Params);
    err |= put_user(p, &data32->pu1Params);
#endif


err |= get_user(p, data->pu1Params);						get_user(compat_uptr_t,  u8 *)
err |= put_user(p, &data32->pu1Params);						put_user(compat_uptr_t,  compat_uptr_t * )

err |= get_user(p, &data32->pu1Params);						get_user(compat_uptr_t,  compat_uptr_t *)
err |= put_user(p, data->pu1Params);						put_user(compat_uptr_t,  u8 *)



十四、C5527整机项目
1. 闪光灯电流和预闪延迟时间不对：闪光灯IC弄错导致，重新按照新的AW3643规格书设置闪光灯的寄存器即可。
2. CCT工具连接不上：修改摄像头预览尺寸。


十五、W5528手机项目部分机器前摄打不开
前后摄都挂在IIC总线2上面，但是前后摄的IIC地址冲突了：
对于后摄S5K3L8：   	.i2c_addr_table = {0x5a, /*0x20,*/ 0xff},
对于前摄IMx190：	.i2c_addr_table = {0x20, /*0x34,*/ 0xff},
上面这个地址冲突没有关系，因为前后摄的MCLK不是共用的，不会相互干扰，前摄打不开的原因是模组本体问题。
以后遇到概率性的问题记得保存现场，不要轻易破坏现场。



十六、编写一个可执行C代码到mtk6739平台上执行：
参考Android源代码情景分析，编译出来后直接adb push到 /system/bin 执行即可。
设置RTC时间：
https://blog.csdn.net/wofeile880_jian/article/details/72312239




十七、P2406 POS项目：进入工厂模式手动测试主照相机没有预览界面。
平台：MT8321
安卓版本：Android5.1
内核版本：kernel-3.10
imgsensor型号：sp0a09，30W

解决：
修改 vendor/mediatek/proprietary/platform/mt6580/hardware/mtkcam/acdk/src/acdk/AcdkMain.cpp ：
mOrientation = 90;
将HAL层的sensor的安装角度从90改成0，有效，但是预览画面旋转90度。
修改成旋转0度时，在kernel log中有camera preview m4u error。
这个项目是横屏，LCM为 240:320，高：宽为 3：4，设置了lcm旋转rotation 90。

在预览画面移动手掌，发现画面移动方向跟实际的移动方向不一致，成90度。拆开sensor，发现模组感光芯片的宽高跟LCD宽高成90度，让模组厂重新打样。

查看sensor数据手册可以知道，sensor寄存器只支持调整：旋转180度，左右镜像，上下镜像。

新模组回来后，发现预览黑屏，在log中找不到跟imgsensor相关的错误log。
解决：
这个POS项目只有后摄，没有闪光灯和前摄，所以要去掉相关的配置。
去掉 kernel flashlight 的配置，因为没有闪光灯。
然后也去掉dws的flashlight和前摄GPIO配置。
同时还要修改摄像头驱动，去掉前摄GPIO配置相关的代码。


工厂模式摄像头自动测试彩条测试：
摄像头打样模组错误，修改寄存器无效。
重新打样模组，然后修改寄存器即可。


十八、POS机项目P2409
平台：MT6260
工作：ESP8266 WiFi模块调试，通过串口给WiFi模块发送AT命令，解析串口读到的数据。
问题：
MT6260通过TXD发AT命令给ESP8266，没有返回值。
通过PC串口工具给ESP8266发AT命令，MT6260的RXD可以收到返回值。
后来通过串口工具发现MT6260的TXD没有数据出来，但是拔掉ESP8266的USB供电接口，MT6260的TXD有数据出来。
分析：
1、怀疑MT6260的串口电压2.8V，ESP8266的串口电压3.3V，怀疑电压不一致导致的。
改成串口1，3.3V通信，也不行。
2、MT6260的TXD连接到ESP8266的RXD，还连到了USB转串口芯片的TXD。然后怀疑WiFi模块内部的USB转串口芯片影响，将那个芯片引脚TX和RX的电阻去掉之后，主机跟WiFi模块串口通信正常了。

```


