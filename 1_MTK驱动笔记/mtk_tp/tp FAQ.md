MTK TP FAQ


```c
FAQ08115
用getevent查看C-TouchPanel上报数据

adb shell
getevent -i
cat /proc/bus/input/devices		查看"Name="mtk-tpd"“项中的”Handlers=“对应哪个event
getevent /dev/input/event5

例如，按下一次tp：
type									code																					value
0003					 				0039 																					0000004c
0003					 				0035 																					000001d0
0003					 				0036 																					00000212
0001					 				014a 																					00000001
0000			 		 				0000 																					00000000
0003			 		 				0039 																					ffffffff
0001				 	 				014a 																					00000000
0000				 	 				0000 																					00000000

3=EV_ABS						0x39(57)   = ABS_MT_TRACKING_ID
1=EV_KEY						0x35(53)   = ABS_MT_POSITION_X								x=0x1d0(464)
0=EV_SYN						0x36(54)   = ABS_MT_POSITION_Y								y=0x212(530)
								0x14a(330) = BTN_TOUCH										Down=1， Up=0
								0x0(0)     = SYN_REPORT										0
											
											


FAQ03862
查看虚拟按键的坐标：
adb shell
cat /sys/board_properties/virtualkeys.mtk-tpd

```

