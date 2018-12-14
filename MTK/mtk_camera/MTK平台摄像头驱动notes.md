


```c

/***********************************************************************************************
*
************************************************************************************************/
platform:mt6761
kernel version:kernel-4.9
Android version:Android 8.1



/***********************************************************************************************
*
************************************************************************************************/
〇、base
1. imgsensor的分辨率：有预览分辨率、拍照分辨率、录像分辨率。
录像size的720P表示1280×720,1080P表示1920×1080。
例如：
imgsensor的datasheet为640×480，表示最大分辨率为30W，宽高尺寸为：w:h=4:3。
通过差值可以增大分辨率。差值是一种算法，它在不生成新的像素的情况下，可以增大图像的分辨率。
新的差值必须是16的倍数、为4:3、16:9或18:9。

2. camera中时钟的含义：
(1) MCLK： 在MIPI CSI接口中，sensor的主时钟，由BB端提供，一般为24MHz或者26MHz。
(2) PCLK：在并行接口中，sensor的像素时钟，该时钟每变化一次，data更新一次。还有VSYNC和HSYNC，用于MCU逐行或逐帧的捕捉像素，工作方式与LCD相反。
MIPI CSI接口不需要PCLK，以MIPI的data时钟连续不断的将sensor的数据传给BB端。

3. PWDN引脚：连接BB端与sensor的 power down引脚，控制sensor的sleep，用来控制sensor进入工作模式还是省电模式的一个引脚。

4. ldo供电电源，由BB端直接提供：
VCAM_AF：音圈马达供电电源，一般1.8V/2.8V
VCAM_IO：数字IO电源，给I2C接口、MIPI接口供电。一般1.8/2.8
VCAM_A： 模拟电源，给感光区和ADC供电。一般1.8/2.8
VCAM_D： 数字内核电源，给ISP供电。一般1.2/1.5/1.8/2.8

5. 防抖
EIS：电子防抖
OIS：光学防抖

6. sensor otp
（1）sensor otp信息存储在sensor芯片内部空间或者sensor外挂的EEPROM中，用于存储sensor的 module ID、lens ID、lens shading、AWB、AF。
（2）这些校准数据用于解决每颗sensor的一致性问题。这些数据由golden 模组提供。
（3）module ID/lens ID：方便对产品进行管理。
（4）lens shading：镜头阴影补偿，用于解决镜头中心和四周光线和色彩不一致的问题，一般会中心亮、四周暗。通过lens shading可以改善这一问题。
（5）AWB：自动白平衡，校正R/G、B/G的值， 改善偏色问题。
（6）AF：用于自动对焦参数校正，改善自动对焦的速度和准确性。
（6）在哪里存储这些信息？
在sensor芯片内部或者外挂EEPROM中。
sensor内部一般使用otp寄存器来保存这些数据，这些寄存器一般只能写一次、读多次。它在sensor内部，所以它和sensor是同一个driver IC。
EEPROM：它和sensor是2个设备，它可以多次读写、它和sensor具有不同的driver IC。

7. 关于前后摄和兼容的问题：
0）前提：
前后摄共用一路iic总线 SCL0/SDA0；
前后摄共用vcama/vcamd/vcamio/MCLK，后摄还有vcamaf；
前后摄的MIPI data 4 line和MIPI clock不是同一IO口；
前后摄的Reset，PWDN不是同一IO口；

1）前后摄可以使用同一颗imgsensor：
例如前后摄的ic都是imx190。
将前摄命名为imx190s(id为imx190的ID加1)，后摄命名为imx190，驱动和效果代码分成2份。
开后摄关前摄，开前摄关后摄：
在给后摄上电的时候，拉高/低前摄的PWDN/Reset，让前摄待机。
在给前摄上电的时候，拉高/低后摄的PWDN/Reset，让后摄待机。
这样通过IIC通信时，前后摄不会相互干扰。

2）后摄1和后摄2兼容的情况，也可以使用同一颗imgsensor：
假如后摄1和后摄2的ic都是gc5025，通过otp的module id来兼容2个相同的ic。

3）什么时候需要区分前后摄iic的干扰？
前后摄的三路电压、MCLK都共用了。当前摄打开时，后摄上电正常并且也处于工作状态，会产生干扰。

4）总结：
IIC通信正常的条件，是IIC slave设备已经上电并且处于工作状态，能够产生响应信号ACK来应答IIC master。
解决方法就是，开后摄关前摄，开前摄关后摄。



/***********************************************************************************************
*
************************************************************************************************/
一. 开机时的camera流程

1. kernel中的probe函数首先执行，包括imgsensor driver，AF driver，flashlight driver。

A. imgsensor driver process：
1) imgsensor驱动作为一个 platform_driver 注册，在dts中定义了
compatible = "vendor,camera_hw"，作为一个 platform_device，然后会执行imgsensor 的 probe 函数。

2) 对于 imgsensor probe 函数：
2.1) 保存已经分配内存的 platform_device 为一个全局变量；
2.2) 注册一个 imgsensor cdev，将 cdev 与 file_operation(open/close/ioctl) 联系起来，创建设备节点 /dev/kd_camera_hw；
2.3) 初始化 imgsensor clock 相关，函数为 imgsensor_clk_init；
2.4) 初始化 imgsensor hw 相关，如MCLK、regulator、GPIO，函数为 imgsensor_hw_init；
2.5) 初始化 imgsensor i2c 相关，函数为 imgsensor_i2c_create；
2.6) 初始化 imgsensor proc 相关，函数为 imgsensor_proc_init；

3) 对于 imgsensor_clk_init 函数：
根据 clock_name ，通过函数 devm_clk_get() 来获取时钟，保存在一个结构体中。

4) 对于 imgsensor_hw_init 函数：
imgsensor 驱动的 hw 分为3部分：gpio，regulator，mclk。它们分别提供了 init()，release()，set() 函数。
imgsensor_hw_init() 这个函数分别调用 gpio_init()，regulator_init()，mclk_init。

4.3) 对于 gpio 部分：
gpio_init(): 调用 pinctrl_lookup_state() 来获取前摄和后摄的 Reset 和 PWDN 引脚状态。
gpio_release(): 调用 pinctrl_select_state() 拉低前后摄的 Reset 和 PWDN 引脚。
gpio_set(): 调用 pinctrl_select_state() 设置前后摄的 Reset 和 PWDN 引脚。
使用 pinctrl_select_state() 时，需要使用自旋锁 DEFINE_MUTEX(pinctrl_mutex)。

4.4) 对于 regulator 部分：
regulator_init(): 调用 regulator_get()。
regulator_release(): 调用 regulator_disable()。
regulator_set(): 调用 regulator_set_voltage()/regulator_enable() 或者 regulator_disable()。
如果前后摄共用 regulator ，在使用 regulator 之后调用 regulator_put() ，使用之前调用 regulator_get()。因为regulator会将使用过的最低电压设置为它的最高电压，导致设置最高电压失败。

4.5) 对于 mclk 部分：
mclk_init(): 调用 pinctrl_lookup_state() 初始化前后摄的 mclk pinctrl。
mclk_release():调用 pinctrl_select_state() 禁止前后摄的 mclk pinctrl。
mclk_set():调用 pinctrl_select_state() 设置前后摄的 mclk pinctrl。

5) 对于 imgsensor_i2c_create 函数：
调用 i2c_add_driver() 添加了3个 i2c_driver：main，sub，main_two。
它们的3个 probe 函数仅仅保存各自的 i2c_client 到一个全局变量中。

6) 对于 imgsensor_proc_init 函数：
调用 proc_create() 创建一些 proc文件接口。

7) 具体的imgsensor 驱动(如 imx214mipiraw.c)：
7.1) 实现了 open/close/ioctl 的具体实现；
7.2) imgsensor 参数的配置和寄存器的配置；

8) 总结
8.1) imgsensor 驱动中，最主要的是 imgsensor cdev file_operation 中的 unlocked_ioctl() 函数，hal 层通过 ioctl() 函数来跟 kernel 层发送和接受数据。



B. AF driver process：
1) main lens 驱动，注册了一个platform_driver和platform_device，然后会执行它的probe函数。
2) 在 platform probe 函数中，它只是调用 i2c_add_driver() 添加一个 lens i2c driver，在dts中有一个节点：compatible = "vendor,CAMERA_MAIN_AF"，然后会进入i2c的probe函数。
3) 在 i2c probe 函数中：
3.1) 首先保存 i2c_client 到全局变量中。
3.2) 然后注册一个 AF cdev ，将它与 file_operation 联系起来，并且创建一个/dev/MAINAF节点。
3.3) 然后初始化 AF的regulator，主要是调用regulator_get()来获取regulator节点并保存。
3.4) 在 AF_Ioctl 中，有一个函数 AF_SetMotorName()，这个函数用来确定当前imgsensor使用的lens，AF的name在hal层与具体的imgsensor配置，通过hal层传入kernel。



C. flashlight driver process：
1)
注册一个 flashlight_core platform_driver ，使用module_init，在dts中有节点： compatible = "vendor,flashlight_core"。

进入 flashlight_core 的 probe 函数：
注册一个 flashlight cdev 设备，将它连接到 file_operation 函数，创建设备节点 /dev/flashlight ，创建一些 device file，如 pt(节能)、strobe、charge、capability。

2）
注册特定闪光灯ic的 platform_driver ，例如注册 lm3643 驱动，dts中有节点：compatible = "vendor,flashlights_lm3643"。

进入 lm3643 的 probe 函数：
初始化 lm3653 pinctrl；
调用 i2c_add_driver 添加一个 lm3643 i2c_driver；

进入 lm3643 i2c probe 函数：
解析dts中的lm3643配置，读取出来并且保存；
初始化2个定时器，超时禁止lm3643的2个channel，关闭闪光灯，这里没有开启定时器，在打闪时候才会开启定时器；
调用 flashlight_dev_register_by_device_id()，保存 lm3643 的 file_operation 结构，将这个 flashlight_dev 添加到 flashlight_list 链表，在 lm3643 file_operation 中完成对它的寄存器读写操作；

3）
hal 层通过ioctl来操作 flashlight_core 的 file_operation：
对于 flashlight_open ，会遍历链表 flashlight_list 上的所有 flashlight ，调用它们的open函数；
对于 flashlight_release ，会遍历链表 flashlight_list 上的所有 flashlight，调用它们的release函数；
对于 flashlight_ioctl ，会在链表 flashlight_list 上搜索符合 type 和 ct 的 ioctl，然后调用ioctl；

4)
flashlight驱动分为2部分：架构相关的驱动和具体的flashlight驱动，通过把具体的flashlight驱动的操作和参数保存到一个链表flashlight_list上，上层只会调用架构相关的驱动。




2. 启动服务 camerahalserver，启动 camerahalserver 进程
(0)在 vendor/xxx/proprietary/hardware/xxxcam/main/hal/service/camerahalserver.rc 
开启 camerahalserver 服务：
service camerahalserver /vendor/bin/hw/camerahalserver

(1)main 函数：
vendor/xxx/proprietary/hardware/xxxcam/main/hal/service/service.cpp

(2)进入CameraProvider构造函数：
hardware/interfaces/camera/provider/2.4/default/CameraProvider.cpp
依次调用：
CameraProvider::CameraProvider();
CameraProvider::initialize();
hw_get_module();
load();
android_load_sphal_library(); //在system/core/libvndksupport/linker.c中定义。
加载库：
/vendor/lib/hw/android.hardware.camera.provider@2.4-impl-xxx.so



3. 启动服务 cameraserver，启动 cameraserver 进程
(1)在system/core/rootdir/init.zygote64.rc中启动：
onrestart restart cameraserver

(2)cameraservice构造函数：
framework/av/services/camera/libcameraservice/CameraService.cpp:
CameraService::CameraService();
CameraService::onFirstRef();
CameraService::enumerateProviders();

framework/av/services/camera/libcameraservice/common/CameraProviderManager.cpp:
CameraProviderManager::initialize();

hardware/xxxcam/main/hal/devicemgr/:
CameraDeviceManagerBase::CameraDeviceManagerBase();
CameraProviderImpl::initialize();
CameraDeviceManagerBase::initialize();
CameraDeviceManagerBase::enumerateDevicesLocked();
CameraDeviceManagerImpl::onEnumerateDevicesLocked();
HalSensorList::searchSensors();
HalSensorList::enumerateSensor_Locked();
SeninfDrv::createInstance();
SeninfDrvImp::SeninfDrvImp();



4、hal层searchSensor做的工作：
1) 首先搜索后摄(id=0)，对配置的每个imgsensor逐一 check_imgsensor_id()。
2) 然后搜索前摄(id=1)，对配置的每个imgsensor逐一 check_imgsensor_id()。
3) 在操作imgsensor之前，先开启imgsensor的ISP模块、MCLK和clock模块、ldo电压，关闭imgsensor之后也要关闭这些。
4) 通过ioctl来读取imgsensor的ID：get_imgsensor_id()，通过iic来读ID，如果能够读到ID，将imgsensor的ID和name保存，传给hal层。
5) 如果imgsensor有otp数据，可以在读到ID之后将imgsensor中的otp数据读出来保存，以后不需要在open camera时再去读otp数据，影响性能。
6) 通过ioctl获取已经搜索到的imgsensor的信息：预览/拍照/录像的分辨率。
7) 对于hal3，还会读取和解析metadata配置的参数，hal1不需要配置metadata数据。metadata用于APP和HAL的参数传递，如分辨率/FPS。
8) 对于已经读到ID的imgsensor，通过ioctl向kernel发送open命令，open imgsensor：
执行平台端otp数据校准，如平台端lsc校准；
执行tsf参数生效，改善color shading；
lens参数生效和FlashAWB闪光灯参数生效；
开启AF线程，需要获取陀螺仪()的数据，控制自动对焦；
发送一些 stopPreview/cancelPicture/close 的命令给camera；



5. 相关的进程
swapper(PID=1): probe 函数在这个进程中进行。
camerahalserver(PID=548):  power on imgsensor, search imgsensor.
initCamdevice(PID=781)：   power on imgsensor, open  imgsensor.


/***********************************************************************************************
*
************************************************************************************************/
二. 打开摄像头流程

1. open camera的相关线程：

1) camerahalserver
打开/关闭ISP模块；
设置PMU mt6370的模式；
控制AF下电，关闭AF的regulator；
控制imgsensor下电，关闭imgsensor相关的regulator，时钟，ISP；
这个进程主要是打开和关闭BB端的ISP模块，它还负责关闭imgsensor和AF。

2) initCamdevice
打开imgsensor：打开相关的时钟，regulator，给imgsensor上电，应用otp。
这个进程负责打开imgsensor。

3) 3ATHREAD
负责flashlight ioctl，控制打闪；
负责imgsensor ioctl，主要是camera效果相关的控制，如gain，flicker，shutter

4) AFthread
负责打开AF驱动，控制AF上电；
控制AF驱动的ioctl，如move AF。

5) Camera@Display
显示线程，控制显示数据的传输；

6) FDClient@Previe
人脸检测线程，负责检测人脸的数据；

7) CamClient@Previ
预览线程，预览数据的传输，预览回调函数；

8) AESenThd
自动曝光线程，控制camera AE；

9) pass1
pass1线程，控制pass1数据传输；

10) pass2
pass2线程，控制pass2数据传输；

11) EventThread
用户空间的事件处理线程。

12) xxx-tpd
内核线程，控制获取TP数据和上报报点；

13) surfaceflinger
显示线程。

14) ISP相关的几个进程
控制BB端的ISP模块；

15) kworker
内核的工作者线程，用来执行工作队列。
工作队列可以通过 INIT_DELAYED_WORK() 或者 INIT_WORK()+queue_work() 来实现。
TP的ESD check，固件升级，上报TP报点都是通过工作队列来实现。

工作队列和内核线程的区别：
工作队列用于执行一些较小的任务，工作队列由工作者线程统一管理。

16) ksoftirqd
内核软终端线程。

17) Binder/HwBinder
binder线程，控制不同线程之间的数据传输；

```





