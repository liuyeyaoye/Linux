

```c

MTK 平台camera移植步骤


平台：MT6739
安卓版本：Android 8.1
内核版本：kernel-4.4


〇、前言

Mediatek平台camera驱动移植涉及到的文件较多，做一个记录，以后点亮新的camera模组按照这个步骤来。



一、硬件信息
1、GPIO引脚
1） imgsensor GPIO
1.1） 型号： 前摄为 imx190s_mipi_raw ，后摄为 imx214_mipi_raw  ， s5k4h7yx_mipi_raw
1.2） 原理图的GPIO：
后摄RST：		GPIO29
后摄PWDN：		GPIO30
前摄RST：		GPIO22
前摄PWDN：		GPIO23
前后摄共用MCLK：GPIO126
前后摄SCL2：		GPIO133
前后摄SDA2：	GPIO132
camera MIPI接口(GPIO154—GPIO163)


2）  flashlight GPIO（无前闪，一般都没有前闪）
2.1） 型号： AW3643CSR
2.2） 原理图的GPIO：
GPIO_FLASH_STROBE(GPIO3)	：	闪光灯驱动IC Flash 脉冲使能脚
GPIO_FLASH_F_EN(GPIO4)		：	手电筒模式使能脚
GPIO_FLASH_EN(GPIO10)		： 	闪光灯驱动IC使能脚



二、驱动配置

1、dws
根据硬件提供的GPIO配置表修改dws文件，配置输入/输出/MIPI.

2、dts
主要是配置pinctrl
2.1) imgsensor
cam0_rst0			cam0_pnd0
cam1_rst0			cam1_pnd0
cam0_mclk_on		cam1_mclk_on
2.2) flashlight
flashlight_en_on				GPIO10
flashlight_strobe_high			GPIO3
flashlight_f_en_low			GPIO4



3、 proj.cfg 
3.1） ProjectConfig.mk


CUSTOM_HAL_IMGSENSOR = imx190s_mipi_raw imx214_mipi_raw s5k4h7yx_mipi_raw gc5025_mipi_raw gc2385_mipi_raw gc0310_mipi_yuv gc0313_mipi_yuv gc2155_mipi_yuv gc2355_mipi_raw
CUSTOM_HAL_MAIN_IMGSENSOR = imx214_mipi_raw s5k4h7yx_mipi_raw gc5025_mipi_raw gc2155_mipi_yuv gc2355_mipi_raw
CUSTOM_HAL_SUB_IMGSENSOR = imx190s_mipi_raw gc2385_mipi_raw gc0310_mipi_yuv gc0313_mipi_yuv
CUSTOM_KERNEL_IMGSENSOR = imx190s_mipi_raw imx214_mipi_raw s5k4h7yx_mipi_raw gc5025_mipi_raw gc2385_mipi_raw gc0310_mipi_yuv gc0313_mipi_yuv gc2155_mipi_yuv gc2355_mipi_raw
CUSTOM_KERNEL_MAIN_IMGSENSOR = imx214_mipi_raw s5k4h7yx_mipi_raw gc5025_mipi_raw gc2155_mipi_yuv gc2355_mipi_raw
CUSTOM_KERNEL_SUB_IMGSENSOR = imx190s_mipi_raw gc2385_mipi_raw gc0310_mipi_yuv gc0313_mipi_yuv
剩下的MAIN2 和 BACKUP（兼容）都设为空。

CUSTOM_HAL_LENS = dw9714af dw9763af lc898217af lc898212xdaf bu6424af ak7371af ad5820af dummy_lens
CUSTOM_HAL_MAIN_LENS = dw9714af dw9763af
CUSTOM_KERNEL_LENS = dw9714af dw9763af lc898217af lc898212xdaf bu6424af ak7371af ad5820af dummy_lens
CUSTOM_KERNEL_MAIN_LENS = dw9714af dw9763af
剩下的SUB 和 BACKUP（兼容）都设为空。


3.2） kernel_config


CONFIG_CUSTOM_KERNEL_IMGSENSOR="imx190s_mipi_raw imx214_mipi_raw s5k4h7yx_mipi_raw gc5025_mipi_raw gc2385_mipi_raw gc0310_mipi_yuv gc0313_mipi_yuv gc2155_mipi_yuv gc2355_mipi_raw"
CONFIG_MTK_IMGSENSOR=y
CONFIG_MTK_FLASHLIGHT=y
CONFIG_MTK_FLASHLIGHT_LM3643=y



4、  两个文件保持一致：
device/mediatek/common/kernel-headers/kd_imgsensor.h
kernel-4.4/drivers/misc/mediatek/imgsensor/inc/kd_imgsensor.h


#define S5K4H7YX_SENSOR_ID                      0x487b
#define SENSOR_DRVNAME_S5K4H7YX_MIPI_RAW        "s5k4h7yx_mipi_raw"




5、  两个文件保持sensor顺序对齐
kernel层：
kernel-4.4/drivers/misc/mediatek/imgsensor/src/common/v1/imgsensor_sensor_list.c


#if defined(S5K4H7YX_MIPI_RAW)
    {S5K4H7YX_SENSOR_ID, SENSOR_DRVNAME_S5K4H7YX_MIPI_RAW, S5K4H7YX_MIPI_RAW_SensorInit},
#endif


HAL层：
vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor_src/sensorlist.cpp


#if defined(IMX190S_MIPI_RAW)
    RAW_INFO(IMX190S_SENSOR_ID, SENSOR_DRVNAME_IMX190S_MIPI_RAW,NULL),
#endif



5.1  HAL层 lenslist.cpp


#if defined(DW9714AF)
        {S5K3L8S_SENSOR_ID, DW9714AF_LENS_ID, "DW9714AF", pDW9714AF_getDefaultData},
#endif


6、   
kernel-4.4/drivers/misc/mediatek/imgsensor/src/common/v1/imgsensor_sensor_list.h 


UINT32 S5K4H7YX_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);


7、 上电时序：  
kernel-4.4/drivers/misc/mediatek/imgsensor/src/mt6739/camera_hw/imgsensor_cfg_table.c


#if defined(S5K4H7YX_MIPI_RAW)
	{
		SENSOR_DRVNAME_S5K4H7YX_MIPI_RAW,
		{
			{SensorMCLK, Vol_High, 0},
			{PDN, Vol_Low, 0},
			{RST, Vol_Low, 1},
			{DOVDD, Vol_1800, 0},
			{AVDD, Vol_2900, 0},
			{DVDD, Vol_1200, 2},
			{AFVDD, Vol_2800, 0},
			{PDN, Vol_High, 0},
			{RST, Vol_High, 0}
		},
	},
#endif


8、将 imgsensor 代码添加到 kernel-4.4/drivers/misc/mediatek/imgsensor/src/mt6739/ 目录即可。如：
kernel-4.4/drivers/misc/mediatek/imgsensor/src/mt6739/s5k4h7yx_mipi_raw


9、 HAL层 imgsensor 效果代码： vendor/mediatek/proprietary/custom/mt6739/hal/imgsensor/s5k4h7yx_mipi_raw/


10、 修改HAL层 featuretable 代码：  vendor/mediatek/proprietary/custom/mt6739/hal/sendepfeature/s5k4h7yx_mipi_raw

```

