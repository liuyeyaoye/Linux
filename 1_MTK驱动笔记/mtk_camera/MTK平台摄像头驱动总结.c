


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

3. PWDN引脚：连接BB端与sensor的 power down引脚，控制sensor的sleep，用来控制sensor进入工作模式还是待机模式的一个引脚。

4. ldo供电电源，由BB端直接提供：
VCAM_AF：音圈马达供电电源，一般1.8V/2.8V
VCAM_IO：数字IO电源，给I2C接口、MIPI接口供电。一般1.8/2.8
VCAM_A： 模拟电源，给感光区和ADC供电。一般1.8/2.8
VCAM_D： 数字内核电源，给BB端的ISP供电。一般1.2/1.5/1.8/2.8

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
使用 pinctrl_select_state() 时，需要使用互斥体 DEFINE_MUTEX(pinctrl_mutex)。

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





/***********************************************************************************************
*
************************************************************************************************/
2. 启动服务 camerahalserver，启动 camerahalserver 进程

(0) 在 vendor/xxx/proprietary/hardware/xxxcam/main/hal/service/camerahalserver.rc 
开启 camerahalserver 进程：
service camerahalserver /vendor/bin/hw/camerahalserver

(1) 进入 camerahalserver 进程的 main 函数：
vendor/xxx/proprietary/hardware/xxxcam/main/hal/service/service.cpp
mian 函数里面配置一下线程池。


(2) 进入 CameraProvider 构造函数：
hardware/interfaces/camera/provider/2.4/default/CameraProvider.cpp
依次调用：
CameraProvider::CameraProvider();
	CameraProvider::initialize();
		hw_get_module(CAMERA_HARDWARE_MODULE_ID,  (const hw_module_t **)&rawModule);
			load();
				android_load_sphal_library(); //在system/core/libvndksupport/linker.c中定义。
				//加载库： /vendor/lib/hw/android.hardware.camera.provider@2.4-impl-xxx.so		
		mModule = new CameraModule(rawModule);
		err = mModule->init();
		mNumberOfLegacyCameras = mModule->getNumberOfCameras();


camera_module_t HAL_MODULE_INFO_SYM __attribute__ ((visibility("default"))) = {
	.common = {
		.tag                			= HARDWARE_MODULE_TAG,
		.module_api_version 	= CAMERA_MODULE_API_VERSION_2_2,
		.hal_api_version    		= HARDWARE_HAL_API_VERSION,
		.id                 			= CAMERA_HARDWARE_MODULE_ID,
		.name               		= "Default Camera HAL",
		.author             		= "The Android Open Source Project",
		.methods            		= &gCameraModuleMethods,
		.dso                		= NULL,
		.reserved           		= {0},
	},
	.get_number_of_cameras 	= get_number_of_cameras,
	.get_camera_info       		= get_camera_info,
	.set_callbacks         		= set_callbacks,
	.get_vendor_tag_ops    		= get_vendor_tag_ops,
	.open_legacy           		= NULL,
	.set_torch_mode        		= NULL,
	.init                  			= NULL,
	.reserved              			= {0},
};




/***********************************************************************************************
*
************************************************************************************************/
3.  启动服务 cameraserver，启动 cameraserver 进程
(1) 在 frameworks/av/camera/cameraserver/cameraserver.rc 中启动：
service cameraserver /system/bin/cameraserver

(2) 进入 cameraserver 进程的 main 函数：
	sp<IServiceManager> sm = defaultServiceManager();
	CameraService::instantiate();//instantiate()在 CameraService 的父类 BinderService 中实现，它的功能是 new CameraService();


(3) 进入 CameraService 构造函数和 onFirstRef() 函数（framework/av/services/camera/libcameraservice/CameraService.cpp）：

CameraService::CameraService();
CameraService::onFirstRef();
	CameraService::enumerateProviders();
		CameraProviderManager::initialize();（framework\av\services\camera\libcameraservice\common）


		//这个 CameraDeviceManagerBase 的 initialize 函数是从哪里被调用的，代码中没有找到......
		CameraDeviceManagerBase::CameraDeviceManagerBase();（vendor\mediatek\proprietary\hardware\mtkcam\legacy\main\hal\devicemgr）
			CameraProviderImpl::initialize();
		CameraDeviceManagerBase::initialize();
			CameraDeviceManagerBase::enumerateDevicesLocked();
			——>CameraDeviceManagerImpl::onEnumerateDevicesLocked()
			————>sensorNum = HalSensorList::searchSensors();
			————————>HalSensorList::enumerateSensor_Locked();
							——>SeninfDrvImp::init();
									mfd = open( "/dev/camera-isp", O_RDWR);
									m_fdSensor = open("/dev/kd_camera_hw", O_RDWR);
			    						mpIspHwRegAddr = (unsigned int *) mmap(0, ISP_HW_RANGE/*0x52000*/, (PROT_READ | PROT_WRITE), MAP_SHARED, mfd, cam_isp_addr/*0x15000000*/);
									......
							——>SeninfDrvImp::setAllMclkOnOff(ISP_DRIVING_8MA, TRUE);
							——>ImgSensorDrv::init();
									m_fdSensor = ::open("/dev/kd_camera_hw", O_RDWR);
							——>for(idx=0; idx<=1; idx++){
									ImgSensorDrv::searchSensor(idx);
										 featureControl(idx, SENSOR_FEATURE_SET_DRIVER, (MUINT8 *)&x, &featureParaLen);
										//会调用kernel的函数：imgsensor_set_driver() 和 imgsensor_check_is_alive(). 唯一的作用是：获取 前摄和后摄 在 sensorList.cpp 中的序号，保存在 x 中。
									querySensorDrvInfo(i);//query sensorinfo
									buildSensorMetadata(i);//fill in metadata
									pSensorInfo = pSensorDrv->getSensorInfo(i);
								}
							——>SeninfDrvImp::setAllMclkOnOff(0, FALSE);
							——>SeninfDrvImp::uninit();
									setSeninf1CSI2(0, 0, 0, 0, 0, 0, 0, 0, 0,0);   // disable CSI2
									munmap(mpIspHwRegAddr, ISP_HW_RANGE)
									close(mfd);
									close(m_fdSensor);
							——>ImgSensorDrv::uninit();
									::close(m_fdSensor);
			————>for(i=0; i<sensorNum; i++){
						......
						sp<PhysEnumDevice> pPhysDevice = new PhysEnumDevice;
	  					pPhysDevice->mSensorName = pHalSensorList->queryDriverName(instanceId);
						......
					}	
			



/***********************************************************************************************
*
************************************************************************************************/
4、hal层searchSensor做的工作：
1) 首先搜索后摄(id=0)，对 kernel.config 配置的每个imgsensor逐一 imgsensor_set_driver() 和 imgsensor_check_is_alive().
2) 然后搜索前摄(id=1)，对 kernel.config 配置的每个imgsensor逐一 imgsensor_set_driver() 和 imgsensor_check_is_alive().
	因为直接使用的 kernel.config 中的 sensor_list 配置，所以 HAL 和 kernel 中的 sensor_list 中的 imgsensor 列表要一致。
3) 在操作imgsensor之前，先开启imgsensor的ISP模块、MCLK和clock模块、ldo电压，关闭imgsensor之后也要关闭这些。
4) 通过ioctl来读取imgsensor的ID：get_imgsensor_id()，通过iic来读ID，如果能够读到ID，将imgsensor的ID和name保存，传给hal层。
5) 如果imgsensor有otp数据，可以在读到ID之后将imgsensor中的otp数据读出来保存，以后不需要在open camera时再去读otp数据，影响性能。
6) 通过ioctl获取已经搜索到的imgsensor的信息：预览/拍照/录像的分辨率。
7) 对于hal3，还会读取和解析metadata配置的参数，hal1不需要配置metadata数据。metadata用于APP和HAL的参数传递，如分辨率/FPS。
8) 对于已经读到ID的imgsensor，通过ioctl向kernel发送open命令，open imgsensor：
执行平台端otp数据校准，如平台端lsc校准；
执行tsf参数生效，改善color shading；
lens参数生效和FlashAWB闪光灯参数生效；
开启AF线程，需要获取陀螺仪(测量手机偏转和倾斜的数据)的数据，控制自动对焦；
发送一些 stopPreview/cancelPicture/close 的命令给camera；




/***********************************************************************************************
*
************************************************************************************************/
5. 关注一下 kernel 中2个重要的函数 imgsensor_set_driver() 和 imgsensor_check_is_alive()
解析 kernel.config 中的 CONFIG_IMGSENSOR 宏：查找 kernel.config 配置的 sensor 在 kdSensorList 中的位置，保存它们的序号到一个数组中。
对所有宏定义中的 sensor ，循环操作：
	调用它的 init 函数，获取具体sensor的操作函数：open/get_info/feature_control/close，
	调用 imgsensor_check_is_alive();
		imgsensor_hw_power(POWER_ON);//根据 sensor 时序操作 gpio/mclk/regulator
		imgsensor_sensor_feature_control( SENSOR_FEATURE_CHECK_SENSOR_ID );
			get_imgsensor_id(...);
		imgsensor_hw_power(POWER_OFF);




/***********************************************************************************************
*
************************************************************************************************/
6. HAL sensor driver 几个重要的类和功能

(1) class SeninfDrvImp
seninf 模块：
	TG前端有一个Seninf Module, Seninf Module的主要功能是处理 Parallel/Mipi/Serial 等 interface 发送的信号。
	TG module全称为Timing Generator，它主要有两个作用，一是产生sensor 工作所需要的 Master Clock，二是接收 sensor 吐出的信号。

sensor data flow :
sensor ——> seninf module ——> TG ——> ISP ——> Raw/YUV.


这个类的作用：
IO地址的map：IspAddr, seninfAddr, MIPIRXAddr, gpioAddr
写一些寄存器的值：seninfReg, ispReg, MIPICSI2Reg
打开/设置/关闭 mclk的频率和驱动电流(24MHz/26MHz):  也是通过写寄存器实现。
 
这个类的主要作用是：配置，设置，操作寄存器，包括 seninf module, TG module, ISP module, MCLK module。



(2) class HWSyncDrvImp
这个类的作用：
用于同步：如时序的同步，中断的同步，各个线程各个模块的同步处理。



(3) class ImgSensorDrv

这个类的作用：

searchSensor : featureControl(sensorIdx, SENSOR_FEATURE_SET_DRIVER, (MUINT8 *)&idx, &featureParaLen);

setScenario：设置各个模式下的参数和传递命令，如设置 FPS ， 设置 HDR 模式等。
sendCommand：featureControl(sensorIdx, FeatureId,  (MUINT8*)pFeaturePara,(MUINT32*)&FeatureParaLen);  与 kernel 中的 feature_control() 对应。
open：featureControl(sensorIdx, SENSOR_FEATURE_OPEN, (MUINT8 *)&featurePara, &featureParaLen);  打开具体的 imgsensor。
close:   featureControl(sensorIdx, SENSOR_FEATURE_CLOSE, (MUINT8 *)&featurePara, &featureParaLen);  关闭具体的 imgsensor。
getInfo: ioctl(m_fdSensor, KDIMGSENSORIOC_X_GET_CONFIG_INFO , &getInfo);  获取具体 sensor 的信息。
getInfo2： ioctl(m_fdSensor, KDIMGSENSORIOC_X_GETINFO2 , &getInfo);
featureControl:   ioctl(m_fdSensor, KDIMGSENSORIOC_X_FEATURECONCTROL , &featureCtrl);
getCurrentSensorType： 获取sensor的格式。
	sensor 有这些格式：  	RAW （RAW_B, RAW_Gb, RAW_Gr, RAW_R）,  一般都是这种格式。
						RAW8 (RAW8_B, RAW8_Gb, RAW8_Gr, RAW8_R),  
						YUV（UYVY, VYUY,	YUYV, YVYU） ，这里都是 YUV422 格式的, 每两个Y共用一组UV分量。
						YCBCR（CbYCrY,	CrYCbY,	YCbYCr,	YCrYCb）
						RAW12,  	RAW14,  RGB888,  RGB565,  JPEG
	YUV格式来源于RGB，它把色彩和亮度分离了，Y表示亮度，UV表示色彩度。
	YCBCR格式来源于YUV，它主要用于数字视频。


这个类的主要作用：
开机枚举搜索 sensor，就下发一条 SENSOR_FEATURE_SET_DRIVER 命令。
对特定的 sensor 进行 ioctl ，如 imx214 的 open/close/get_info/feature_control。




(4) class HalSensorList
各种 query() 函数：如 query sensor ID/name/type，info, number，metadata等。
buildStaticInfo()：解析保存 metadata，包括： "LENS",  "SENSOR",  "TUNING_3A",  "FLASHLIGHT",  "SCALER",  "FEATURE",  "CAMERA",  "REQUEST", 这些信息都是在 HAL 的 custom 目录里面配置好了的。
enumerateSensor_Locked()：开机时枚举 sensor ，获取了前后 sensor 在 sensorList.cpp 数组中的序号，然后保存起来了。在枚举了 sensor 之后就会调用 buildStaticInfo() ， 解析 metadata。
querySensorInfo： 获取 sensor info, 通过 ImgSensorDrv->sendCommand() 来实现的。





(5) class HalSensor
powerOn 和 powerOff 函数：开关MCLK，设置MCLK驱动电流，调用 mpSensorDrv->open(sensorIdx) 打开 sensor 。
configure:  配置不同模式下的sensor参数。
sendCommand： 相当于 HAL 的 ioctl ，它会根据传入的 cmd 调用 ImgSensorDrv->sendCommand() 和 SeninfDrvImp->sendCommand() ，比如设置/获取帧率等。
setSensorMclkDrivingCurrent：调用 ImgSensorDrv->sendCommand() 和 SeninfDrvImp->setMclkIODrivingCurrent() 来实现。
seninfControl:  调用 SeninfDrvImp->configSensorInput()。

这个类的主要作用是：将需要 ImgSensorDrv 和 SeninfDrvImp 的操作合并起来一起控制。






/***********************************************************************************************
*
************************************************************************************************/
7. 相关的进程
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
设置PMU xx6370的模式；
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



/***********************************************************************************************
*
************************************************************************************************/
2.  camera preview flow
(0) 
DisplayClient 负责显示图像数据， 它会把图像数据送往 Surface 显示。
CameraAdapter 负责提供图像数据。它包含了多个 CamNode ，不同的 CamNode 描述不同的 buffer 处理。

(1) 控制流 ：使用不同的 Node 来描述不同的 buffer 处理
Pass1Node： 负责和 Sensor Driver、ISP Driver 打交道，进入预览模式的重点工作都由它来完成。
Pass2Node
DefaultCtlNode
CamGraph : 作为各个 Node 通讯的桥梁，CamGraph 代表了整个系统， 所有的 Node 都需要连接到 CamGraph
各个Node之间的通讯就需要用到 connectData 和 connectNotify 函数， connectData 为两个 node 之间 buffer 传输的连接，而 connectNotify 为两个 node 之间消息传输的连接。例如：

connectData( PASS1_RESIZEDRAW, CONTROL_RESIZEDRAW, mpPass1Node, mpDefaultCtrlNode);
Pass1Node 和 DefaultCtrlNode 就连接在一起，事件是 PASS1_RESIZEDRAW，也就是说当 Pass1Node 调用 handlePostBuffer(PASS1_RESIZEDRAW, buffer) 的时候，DefaultCtrlNode 里面的 onPostBuffer 函数将会接受到 Pass1Node 的 buffer。

connectNotify( PASS1_START_ISP, mpPass1Node, mpDefaultCtrlNode)，事件是 PASS1_START_ISP，当 Pass1Node 调用 handleNotify(PASS1_START_ISP) 的时候，DefaultCtrlNode 里面的 onNotify 函数将会接收到 PASS1_START_ISP 消息。

connectData 和 connectNotify 的不同之处在于，一个可以传输整个buffer，但只能一对一连接，一个只能传输消息，但可以一对多连接。


(2) data flow
pass1 ——> defaultNode ——> pass2 ——> Display


(3) DisplayClient
它会创建一个 DisplayThread，一个 ImgBufQueue。
ImgBufQueue 里有两个 Buf 队列，mTodoImgBufQue 和 mDoneImgBufQue 。
prepareOneTodoBuffer 函数做的事情就是从 dequePrvOps 函数 deque 出 StreamImgBuf，并用它生成 ImgBufQueNode，
把 ImgBufQueNode 的标志位设 eSTATUS_TODO 后调用 ImgBufQueue 的 enqueProcessor 函数把所有的 ImgBufQueNode 都放入到 mTodoImgBufQue 做接收数据的准备。

DisplayClient 准备好 buffer 放到 mTodoImgBufQue 里面。 
Pass1Node 从底层 deque 一帧数据，然后将数据 post 给 DefaultCtrlNode，DefaultCtrlNode 又将数据 post 给 Pass2Node。 
Pass2Node 保存好 buffer 之后会触发 threadLoopUpdate，threadLoopUpdate 通过 DefaultBufHandler 从 mTodoImgBufQue 取出buffer，再将 buffer 交给 IHalPostProcPipe 处理，
当 IHalPostProcPipe 处理完之后会回调 Pass2CbFunc 函数，Pass2CbFunc 通过 DefaultBufHandler 把 buffer 放回 mDoneImgBufQue 里面。 
最后 DisplayClient 不断从 mDoneImgBufQue 里面取出已经处理好的 buffer 送到 Surface 里面




/***********************************************************************************************
*
************************************************************************************************/
三. 总结

1. 在 mt6761 平台上, 对于前摄和后摄, 只创建了1个字符设备结点: /dev/kd_camera_hw , 只有1个 file_operation 结构(open/release/unlocked_ioctl)，那么如何区分HAL层打开的是前摄还是后摄？

(1) 看下HAL层操作imgsensor的流程, 在 imgsensor_drv.cpp 中: 
	fd = open("/dev/kd_camera_hw", O_RDWR);
	featureCtrl.InvokeCamera = sensorIdx;
	......
	ioctl(fd, SENSOR_FEATURE_SET_DRIVER , &featureCtrl);
	......
在开机 searchSensor 时, sensorIdx 分别被赋值为 0(main), 1(sub), 2(main2)......，HAL层把 InvokeCamera 这个参数传给 kernel层，用来区分前后摄。

(2) 在 kernel 层的 imgsensor.c 中:
	psensor->inst.sensor_idx = pFeatureCtrl->InvokeCamera;
	drv_idx = imgsensor_set_driver(psensor);
使用 sensorIdx ，可以区分前后摄的 i2c driver/gpio/mclk/regulator，区分前后摄的上下电。
所以 cdev 和 file_operation结构可以只有一个，通过 sensorIdx 来区分。

2. imgsensor 的 gpio 配置
(1) 包括 pinctrl 和 regulator 配置:
pinctrl包括：PWDN, Reset, MCLK.
regulator包括：VCAMA, VCAMD, VCAMIO, VCAMAF.

3. imgsensor i2c
(1) imgsensor 前后摄 i2c_driver 的 probe() 函数只做了一件事：保存 i2c_client 为一个全局变量。


4. camera帧率
(1) 曝光：让sensor感光，让它积累的电信号转换为数字信号。sensor曝光是一行一行的进行的，所以 dummyLine（无效行）会增加曝光时间。
(2) 快门：控制sensor的曝光时间。代码中的 shutter 表示曝光行数。
(3) FrameLength：一帧的帧长，可以理解为一帧的行数。FrameLength= shutter + dummyLine 。
(4) 帧率计算方式：FrameLength = pclk / framerate * 10 / lineLength ， framerate = (10*pclk) / (linelength * FrameLength) 。
(5) linelength：在不同模式下(预览/拍照/录像)，sensor一行的曝光时间，是固定值。pclk 也是固定值。FrameLength 是可变的。
(6) 计算帧率：在预览模式下，pclk = 320000000，linelength = 5008，framelength = 2110，算出 framerate =302，30帧。
(7) kernel 中只能配置最大帧率为 30 帧，它提供了设置帧率的函数（实际上是根据帧率计算出帧长，然后将帧长写入寄存器）。
具体需要设置多大的帧率是app设置的。在 HAL 的 fbtl 中可以配置几个固定的帧率：5/15/20/24/30.


5. camera flicker
原因： sensor在日光灯作为光源下获取图像数据时会产生 flicker，原因是照在不同sensor像素上的光能量不同造成的。
电源的频率有50Hz和60Hz的正弦波，对应的能量是一个100Hz和120Hz的波形。
曝光是一行一行的进行，每个像素的曝光时间都是一样的。同一行的所有点接受的能量都是一样的，不同行接收到的能量是不一样的。
避免闪烁就是找到一个条件让不同行接收到的能量是一样的，条件是曝光时间必须是光能量周期的整数倍时间。
如果在代码中使能了 autoflicker ，那么在设置帧率的时候，会根据帧率的值来设置一个符合避免 flicker 的帧率，例如：
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	......
在 HAL 的 ftbl中可以配置 flicker 模式为：OFF/50Hz/60Hz/auto。


```





