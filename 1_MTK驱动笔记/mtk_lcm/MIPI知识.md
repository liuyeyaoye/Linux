


```c


一． MIPI：移动产业处理器接口
1. MIPI的特点：
LP（低功耗）模式， HS（高速）模式， EMI（抗辐射）， 低成本物理层。


二． MIPI  Dsiplay工作组下有以下协议：
DCS：显示命令接口
DBI：显示总线接口
DPI：显示像素接口(数字并行接口，用在HDMI, MHL)
DSI：显示串行接口
CSI：显示摄像接口
D-PHY物理层

1． DCS： 是一个标准化的命令集，用于命令模式的显示模组。
2． DBI：也称为MPU模式，数据线和控制线复用，在 DBI  LCM  Panel中有  FB帧缓冲区。
3． DPI：也称为RGB模式，采用SPI，数据线和控制线分离，在 Host  Processor中有帧缓冲区FB。
4． DSI：定义了一个位于处理器和显示模组之间的高速串行接口。
DSI的操作模式有：Command mode（类似于 MPU），Video moed（类似于 RGB）：同步脉冲模式，同步事件模式，Burst模式。
DSI的Command模式：采用发送命令和数据到具有FB的控制器，主机通过命令控制显示器，采用双向接口；
DSI的Video模式：从主机传输到外设采用实时像素流，高速传输，可能只有一个单向数据路径。外部LCM模组一般为Video模式，但是在LCM初始化时使用Command模式来读写寄存器，因为在低速下数据不易出错而且容易测量。
5． CSI：定义了一个位于处理器和摄像模组之间的高速串行接口。
6． D-PHY：提供DSI和CSI的物理层定义。


三． D-PHY物理层
1. D-PHY包括DSI（串行显示）物理层和CSI（串行摄像）物理层的定义。
2. D-PHY包括LP模式和HS模式的物理层定义。
3. 一个D-PHY的配置包括：一个时钟lane，一个或者多个数据Lane。
例如：2个Lane的PHY的配置为：
       时钟Lane（单向）+  数据Lane（双向）+  数据Lane（双向）


四． MIPI的数据传输模式
1. 低功耗LP模式
在此模式下，只能使用Lane0通道，时钟由DP和DN异或得到。
2. 高速HS模式
DSI的Video模式必须使用高速模式


五． MIPI的数据包类型
1. 短帧结构：4字节（固定长度）
长帧结构：6~65541字节（可变长度）
2. 短数据包结构
数据包包头（4字节）：
数据标示符DI（1字节）：包含虚拟数据通道[7:6]和数据类型[5:0]
数据包（2字节）：要传送的数据
误差校正码ECC（1字节）：可以把两个位的错误纠正



3. 长数据包结构
(1)数据包包头（4字节）：
数据标示符DI（1字节）：包含虚拟数据通道[7:6]和数据类型[5:0]
字数WC（2字节）：传送数据的长度（要传输WC个字节的数据）
误差校正码ECC（1字节）：可以把两个位的错误纠正
（2）有效传送数据（0~65535字节）
（3）数据包页脚（2字节） 校验码
如果数据包的有效长度为0，则校验码为：0xFFFFh
如果校验码不能计算，则校验码为：0x0000h
数据包的长度为：6~65541 bytes



/*****************************************************************************************************************************
*
******************************************************************************************************************************/
六. 总结

1. 显示模组的常用接口
(1) MIPI(移动产业处理器接口)协议接口：
DCS：DCS 是一个标准的命令集，用于命令模式的显示模组。
DBI：显示总线接口，也叫MCU接口，处理器通过并行接口传输命令和数据，并向LCM自带的GRAM更新数据实现LCM的刷新。
DPI：显示像素接口，也叫RGB接口，采用普通的同步/时钟/信号线来传输数据，采用SPI等控制线来传输命令。
	DPI和DBI的最大区别：DBI数据线和控制线复用，DPI数据线和控制线分离。
DSI：显示串行接口，高速，串行接口。
CSI：摄像串行接口，高速，串行接口。
D-PHY：提供DSI和CSI的物理层定义。D-PHY支持LP和HS两种传输模式。
	LP模式：单端信号，用于传输控制命令，功耗小，速率低(<10Mbps)。它只使用数据lane0传输数据。
	HS模式：低压差分信号，用于高速传输数据，功耗大，速率高(80Mbps ~ 1Gbps/Lane)。
D-PHY协议规定最小传输单位是1个字节。
发送数据时低位在前，高位在后。

(2) RGB接口，LCD分辨率在 800*480以下，大部分处理器均支持这种接口。

(3) LVDS接口，低压差分信号传输，通过芯片将RGB转换成LVDS接口，优点是低噪声和低功耗。

(4) eDP接口，支持超高分辨率。



2. MIPI DSI接口
(1) 1条时钟lane，1—4条数据lane。时钟lane是单向的，数据lane可以是单向或者双向的。
(2) 数据lane在LP模式是双向的，在HS模式一般是单向的。所以BB端读LCM寄存器的值做ESD时，必须在LP模式下进行。
(3) DSI的操作模式：
command mode:类似于MCU接口，发送数据和命令到具有显示缓存的控制器，采用双向接口。一般使用LP模式传输。
video mode: 它只能采用HS模式传输。它支持3种格式的数据包传输：
同步脉冲模式：DSI基于各种不同的同步数据包来做数据同步。这种数据包括：重构，时间校准等。
同步事件模式：这种模式和第一种模式很像，但是这种模式不会发重构和时间校准的数据包，它们只发送一种叫做”Sync event”的包。
burst模式： 在 horizontal 的时序是一样的情况下DSI会把连接的速度提升到Panel支持的最大速度。在这种模式下发送RGB数据包的时间被压缩，以留出更多的时间用来传送其他的数据。
为了使能video模式，处理器需要发送不同的包到panel，用来设置开始和结束的porch，下面是video模式常用的数据包：
VSS:  DSI Sync Event Packet: V Sync Start
VSE:  DSI Sync Event Packet: V Sync End
BLLP: DSI Packet: Arbitrary sequence of non-restricted DSI packets or Low Power Mode incluing optional BTA.
HSS:  DSI Sync Event Packet: H Sync Start
HAS:  DSI Blanking Packet: Horizontal Sync Active or Low Power Mode, No Data
HSE:  DSI Sync Event Packet: H Sync End
HFP:  DSI Blanking Packet: Horizontal Front Porch or Low Power Mode
HBP:  DSI Blanking Packet: Horizontal Back Porch or Low Power Mode
RGB:  DSI Packet: Arbitrary sequence of pixel stream and Null Packets
LPM:  Low Power Mode incuding optional BTA




(4) 帧类型
短帧：4字节（固定）
	数据标识(Data ID)，1字节
	帧数据，2字节
	错误检测ECC，1字节

长帧：6—65541字节
帧头部（4字节）
	数据标识(Data ID)，1字节
	数据计数，2字节（数据填充的个数）
	错误检测ECC，1字节
帧数据（0—65535字节）
帧尾：校验和（2字节）



3. 即使外部LCM是video mode，但是在LCM初始化时使用command mode来读写寄存器，因为低速下数据不易出错且容易测量。


```


