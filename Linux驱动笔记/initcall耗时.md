
```c
       120.717846 :    1-swapper/0       : initcall: of_init    21.670154ms
       142.905154 :    1-swapper/0       : initcall: init_mtk_governor    15.895924ms
       181.379000 :    1-swapper/0       : initcall: customize_machine    37.324770ms
       215.174462 :    1-swapper/0       : initcall: param_sysfs_init    20.557769ms
       240.708846 :    1-swapper/0       : initcall: pm_sysrq_init    25.463769ms
       314.744693 :    1-swapper/0       : initcall: event_trace_init    42.040385ms
       462.944308 :    1-swapper/0       : initcall: populate_rootfs   129.648078ms
       554.023924 :    1-swapper/0       : probe: probe=platform_drv_probe drv=musb-hdrc(c115d4f4)    16.962923ms   			//obj-$(CONFIG_USB_MTK_HDRC) += usb20/
       554.110155 :    1-swapper/0       : initcall: musb_init    17.804154ms													//usb20/musb_core.c:2725:  module_init(musb_init);
       585.410847 :    1-swapper/0         : probe: probe=mtkfb_probe drv=mtkfb(c116569c)    27.301615ms						//obj-$(CONFIG_MTK_FB) += video/
       585.736539 :    1-swapper/0       : initcall: mtkfb_init    28.259846ms													//video/common/mtkfb.c:2879:  module_init(mtkfb_init);
       620.816155 :    1-swapper/0       : probe: probe=platform_drv_probe drv=cldma_modem(c1171104)    17.007385ms			//obj-$(CONFIG_MTK_ECCCI_DRIVER) +=  eccci/
       621.514309 :    1-swapper/0       : initcall: modem_cd_init    18.555692ms
       679.580386 :    1-swapper/0       : initcall: battery_meter_init    15.156077ms
       731.987232 :    1-swapper/0       : probe: probe=platform_drv_probe drv=mtk-msdc(c11c8a5c)    48.688077ms
       782.236848 :    1-swapper/0       : probe: probe=platform_drv_probe drv=mtk-msdc(c11c8a5c)    50.140692ms
       784.621232 :    1-swapper/0       : initcall: mt_msdc_init   101.616000ms
       838.636001 :    1-swapper/0       : probe: probe=platform_drv_probe drv=soc-audio(c11cff84)    16.032385ms
       838.761155 :    1-swapper/0       : initcall: mt_soc_snd_init    16.380846ms
      1461.981465 :    1-swapper/0         : probe: probe=i2c_device_probe drv=LTR559_ALSPS(c1157b7c)   602.935078ms		//obj-$(CONFIG_CUSTOM_KERNEL_ALSPS) += alsps/
      1462.388003 :    1-swapper/0       : initcall: alsps_init   604.432694ms
      1525.427311 :    1-swapper/0       : probe: probe=i2c_device_probe drv=MXC400X(c1157e70)    62.731692ms			//obj-$(CONFIG_CUSTOM_KERNEL_ACCELEROMETER) += accelerometer/
      1525.797388 :    1-swapper/0       : initcall: acc_init    63.331308ms
      2166.361543 :    6-kworker/u8:0    : probe: probe=i2c_device_probe drv=msg2xxx(c1182c40)   632.840309ms
      2167.105312 :    6-kworker/u8:0    : probe: probe=platform_drv_probe drv=mtk-tpd(c1181eec)   633.933232ms
      2171.522005 :    1-swapper/0       : initcall: battery_init   638.496848ms
      2175.993697 :    1-swapper/0       : Kernel_init_done
```


