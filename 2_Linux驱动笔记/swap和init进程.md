
1. linux 中的 swap 和 init 进程：

1). swap 进程的 PID=0,它也叫 idle 进程。它是所有进程的祖先。 

2). swap 进程是 linux 内核初始化阶段时从无到有创建的内核进程，linux kernel 的驱动 probe 函数都在 swap 进程中执行。

3). swap 进程使用的是静态分配的数据结构，其他进程使用的是动态分配的数据结构。

4). swap 进程会调用 start_kernel() 函数来初始化内核，激活中断，创建一个PID=1的内核进程：init进程。swap 和 init 进程共享进程资源。

5). 多处理器中，每个CPU都拥有自己的 swap 进程。

6). init进程会继续执行内核初始化，然后执行 execve() 系统调用执行 /sbin/init 应用程序，这时，init 进程拥有了属于自己的进程资源。

7). 在操作系统关闭之前，init 进程一直存在，它创建和监控其他进程的活动。

2. 看一下 kernel log 中的 swap 进程 和 init 进程

1). swap 进程：

```c
[    2.296150] <2>.(3)[1:swapper/0][camerahw_probe] start
[    2.299506] <2>.(3)[1:swapper/0]kd_camera_hw supply vcama_main2 not found, using dummy regulator
[    2.299555] <2>.(3)[1:swapper/0]kd_camera_hw supply vcamd_main2 not found, using dummy regulator
[    2.299596] <2>.(3)[1:swapper/0]kd_camera_hw supply vcamio_main2 not found, using dummy regulator
[    2.299637] <2>.(3)[1:swapper/0]kd_camera_hw supply vcama_sub2 not found, using dummy regulator
[    2.299673] <2>.(3)[1:swapper/0]kd_camera_hw supply vcamd_sub2 not found, using dummy regulator
[    2.299716] <2>.(3)[1:swapper/0]kd_camera_hw supply vcamio_sub2 not found, using dummy regulator
[    2.300272] <2>.(3)[1:swapper/0]mclk_init : pinctrl err, (null)
[    2.300275] <2>.(3)[1:swapper/0]mclk_init : pinctrl err, (null)
[    2.300278] <2>.(3)[1:swapper/0]mclk_init : pinctrl err, (null)
[    2.300280] <2>.(3)[1:swapper/0]mclk_init : pinctrl err, (null)
[    2.300653] <2>.(3)[1:swapper/0]imx214_EEPROM_FMT[CAM_CAL_init] CAM_CAL_i2C_init
[    2.300657] <2>.(3)[1:swapper/0]imx214_EEPROM_FMT[RegisterCAM_CALCharDrv] RegisterCAM_CALCharDrv
[    2.300749] <2>.(3)[1:swapper/0]imx214_EEPROM_FMT[CAM_CAL_init]  Attached!! 
[    2.704036] <2>.(2)[1:swapper/0]MAINAF [AF_i2c_probe] Start
[    2.704738] <2>.(2)[1:swapper/0]MAINAF [Register_AF_CharDrv] Start
[    2.705533] <2>.(2)[1:swapper/0]CHRDEV "MAINAF" major number 232 goes below the dynamic allocation range
[    2.706803] <2>.(2)[1:swapper/0]MAINAF [Register_AF_CharDrv] End
[    2.707559] <2>.(2)[1:swapper/0]MAINAF [AFRegulatorCtrl] AFIOC_S_SETPOWERCTRL regulator_put           (null)
[    2.709361] <2>.(2)[1:swapper/0]MAINAF [AFRegulatorCtrl] [Init] regulator_get ffffffcbbb916580
[    2.710440] <2>.(2)[1:swapper/0]MAINAF [AF_i2c_probe] Attached!!
[    2.934025] <0>.(3)[1:swapper/0]flashlights_mt6370: mt6370_parse_dt: Channel number(2).
[    2.934037] <0>.(3)[1:swapper/0]flashlights_mt6370: mt6370_parse_dt: Parse dt (type,ct,part,name,channel,decouple)=(0,0,0,flashlights-mt6370,0,0).
[    2.934042] <0>.(3)[1:swapper/0]flashlights_mt6370: mt6370_parse_dt: Parse dt (type,ct,part,name,channel,decouple)=(0,1,0,flashlights-mt6370,1,0).
[    2.934584] <0>.(3)[1:swapper/0]flashlight: flashlight_dev_register_by_device_id: Register device (0,0,0)
[    2.934588] <0>.(3)[1:swapper/0]flashlight: flashlight_dev_register_by_device_id: Register device (0,1,0)
```
2) init 进程：
```c
	Line 67: <14>[  112.647956] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[free] Done
	Line 68: <14>[  112.654087] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[dumping] Done
	Line 69: <14>[  112.664591] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[free] Done
	Line 70: <14>[  112.667748] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[free] Done
	Line 71: <14>[  112.672901] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[dumping] Done
	Line 72: <14>[  112.688058] .(0)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[free] Done
	Line 74: <14>[  112.728992] .(3)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[free] Done
	Line 75: <14>[  112.731820] .(3)[1:init]init: PropSet [debug.mtk.aee.vstatus]=[dumping] Done
  ```
  
  

