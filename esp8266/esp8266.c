/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
一. ESP8266-DevKitC 入门指南.pdf
1. 使用的是 ESP8266-DevKitC 开发板。
2.
Bit1=OFF,Bit2=ON,选择自动下载功能;
Bit1=ON,Bit2=OFF,选择流控功能。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
二. ESP8266EX 技术规格书.pdf
1. ESP8266EX 只需通过 SPI/SDIO 接口或 UART 接口即可作为 Wi-Fi 适配器,应用到基于任何微控制器的设计中。

2. WiFi
安全机制 WPA/WPA2
加密类型 WEP/TKIP/AES

3. ESP8266EX 芯片内置了存储控制器,包含 ROM 和 SRAM。
当 ESP8266EX 运行在 Station 模式下,连上 AP 后,在 Heap + Data 区用户可用 SRAM 空间最高为 50 kB。

4. UART接口
UART0 可以用用做通信接口,支持流控。os_printf() 默认从 UART0 打印。
UART1 目前只有数据传输功能,所以一般用作打印 log。
不管什么情况，U0TXD默认上电有系统打印，对此敏感应用可通过UART的内部引脚交换功能，在初始化的时候，调用system_uart_swap函数。
将txd和rxd 分别于U0RTS（MTDO/GPIO15）、U0CTS (MTCK/GPIO13)交换来屏蔽该上电的系统打印。 
交换后，硬件上的下载管脚还是使用U0TXD + U0RXD，通信时需要将 MTDO 对应接到MCU的 RXD,MTCK 对应加到MCU的 TXD。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
三. ESP8266 技术参考手册.pdf
这个PDF描述了ESP8266EX 各个接口的介绍，配置，说明。包括：寄存器定义，GPIO，SDIO，SPI，HSPI，I2C，I2S，UART，PWM，IR红外，Sniffer。
这些外设接口都是复用GPIO的。

1. UART
UART0 和 UART1接口的数据传输均由硬件实现。数据传输速度可达到 115200*40(4.5Mbps）。
UART0 可以用作通信接口，支持流控。
UART0 默认会在上电启动期间输出一些打印log。
UART1 只有数据传输功能，用作打印log。
UART0 有 TX、RX，可做数据传输.
UART1 由于 RX 脚被 SPI-Flash 占用，只能使用 TX，可以做串口调试信息打印。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
四. ESP8266-SDK 入门指南.pdf
这个PDF描述了开发板的启动， SDK 的编译，固件 bin 的下载。

1. ESP8266 Firmware for Non-FOTA
blank.bin：初始化系统参数，在 SDK 根目录中提供。至少需要烧录一次。2M flash的下载地址：0x1FB000
esp_init_data_default.bin：初始化射频参数,在 SDK 根目录中提供。至少需要烧录一次。2M flash的下载地址：0x1FC000
blank.bin：初始化系统参数，在 SDK 根目录中提供。至少需要烧录一次。2M flash的下载地址：0x1FE000
eagle.flash.bin：主程序，编译代码生成。2M flash的下载地址：0x00000
eagle.irom0text.bin：主程序，编译代码生成。2M flash的下载地址：0x10000

2. P18 
eagle.irom0text.bin 的 addr 和 len 可以修改，在这里面查看：
ESP8266_NONOS_SDK/ld/eagle.app.v6.ld 中的 irom0_0_seg 。

3. P19
Non-FOTA 固件的下载地址：各个bin文件的下载地址。

4. P21
在 ubuntu 下编译 SDK。

5. P25
在 windows 中下载 bin文件到ESP8266。

6. Non-OS SDK 是不基于操作系统的 SDK,提供 IOT_Demo 和 AT 的编译。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
五. ESP8266 Non-OS SDK API 参考
这个PDF描述了ESP8266各个接口如WiFi接口函数的声明和使用。

1. Non-SDK 使用4类函数（P19）
应用函数：建议添加 ICACHE_FLASH_ATTR 宏，相应程序将存放在 flash 中，被调用时才加载到 cache 运行。如果添加了 IRAM_ATTR 宏的函数，则会在上电启动时就加载到 iRAM 中。
回调函数：
用户任务：0、1、2。任务优先级为 2 > 1 > 0。即 Non-OS-SDK 最多只支持 3 个用户任务，优先级分别为 0、1、2。
中断服务程序（ISR）：ISR 必须添加宏 IRAM_ATTR。

2.
添加了了 ICACHE_FLASH_ATTR 的代码通常比使用 IRAM_ATTR 标记的代码执行得慢。然而,像大多数嵌入式平台一样,ESP8266 的 iRAM 空间有限,因此建议一般代码添加
ICACHE_FLASH_ATTR,仅对执行效率要求高的代码添加 IRAM_ATTR 宏。

3.
ESP8266 带有 160 KB 的 RAM,其中 64 KB 为 iRAM,96 KB 为 dRAM。
iRAM 进一步分成两块：
32 KB iRAM 块运行标有 IRAM_ATTR 的代码
另一个 32 KB 块用作cache,运行标有 ICACHE_FLASH_ATTR 的代码。

4. 
WiFi接口：P49

5. 
I2C Master接口，P157
ESP8266 不能作为 I2C 从设备,但可以作为 I2C 主设备,对其他 I2C 从设备(例如大多数字传感器)进行控制与读写。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
六. ESP-WROOM-02D/02U 技术规格书
ESP-WROOM-02D 和 ESP-WROOM-02U 配置的SPI Flash的大小： 2 MB（2048 KB）（spi ota map： 16Mbit： 512 KB+512 KB，16Mbit-C1：1024 KB+1024 KB） 的 。使用16Mbit-C1。
从 ESP8266_NONOS_SDK_V3.0.0, AT_V1.7 起, 由于 AT bin size 增大,AT 默认只支持 1024 KB + 1024 KB 的 flash map。
支持的 SPI 模式包括:Standard SPI、DIO (Dual I/O)、DOUT (Dual Output)、QIO (Quad I/O) 以及 QOUT(Quad Output)。
ESP-WROOM-02D 和 ESP-WROOM-02U 使用用 26 MHz 晶振。所以打印串口log的波特率为 74880。
/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
1. Wifi 的 station模式和SoftAP 模式
station模式：ESP8266 模块相当于一个客户端，可以连接到其他路由器发出的WIFI信号。
SoftAP模式：ESP8266 模块本身相当于一个路由器，其他设备可连接到该模块发送的信号。
Station+SoftAP 模式： ESP8266 模块可在与其他设备连接的同时充当路由器。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
AT指令集开发：
1. ESP8266 本身就是一个mcu，它可以有2个用途：
(1）把它当成一个普通的wifi模块，用官方集成的AT指令集进行开发。
(2）把它当成一个集成了wifi功能的mcu，用它提供的SDK进行开发。

2. 若下载 ESP8266 AT 固件或者使用用开发板或模组中默认的固件,在运行模式下,系统的打印信息末尾显示“Ready”字样。
在终端输入指令“AT”,系统显示“OK”,代表固件正常运行。
因 AT 固件强制设置波特率为 115200,与 ESP8266 默认的波特率 74880 不符,因此系统初始化的信息会显示为乱码,只要最后显示“Ready”字样,均为正常情况。
UART0 默认会在上电启动期间输出⼀一些打印，此期间打印内容的波特率与所⽤用的外部晶振频率有关。
使⽤用 40 MHz 晶振时，该段打印波特率为 115200；使⽤用 26 MHz 晶振时，该段打印波特率为 74880

3. AT.bin下载步骤：
Flash size 16Mbit-C1，16 Mbit-C1 Flash，Map: 1024 KB + 1024 KB。

blank.bin                   			0x1FB000
esp_init_data_default_v08.bin   	0x1FC000
blank.bin					0xFE000
blank.bin					0x1FE000
boot_v1.7.bin              			0x00000
user1.2048.new.5.bin        		0x01000

4. tools下载步骤
将 botton2 置为 on。
USB连接ESP8266
打开tools，不要勾选 donotchgbin ，点击start
点击stop
拔掉USB重新连接
点击start

windows下串口连接不上的问题：
打开虚拟机的USB设备筛选器，删除ESP8266的串口设备，不要默认勾选。在需要时在勾选。
或者先在linux的minicom中连接串口，再在虚拟机中连接串口。
超级终端乱码：改成GB2312显示。



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
AT指令


1. 复位
AT+RST\r\n


2. 测试 AT 启动，返回 OK
AT\r\n
return: 
AT\r\n\r\nOK\r\n


3. 查看版本信息
AT+GMR\r\n
return:
AT+GMR\r\nAT version:1.7.0.0(Aug 16 2018 00:57:04)\r\n
SDK version:3.0.0(d49923c)\r\n
compile time:Aug 23 2018 16:58:12\r\n
Bin version(Wroom 02):v1.7.0\r\nOK\r\n


关闭回显
ATE0\r\n


恢复出厂设置，将擦除所有保存到 Flash 的参数,恢复为默认参数，机器重启。
AT+RESTORE\r\n


设置 UART 当前临时设置,不保存到 Flash
AT+UART_CUR=115200,8,1,0,0\r\n


设置 UART 配置,保存到 Flash
AT+UART_DEF=115200,8,1,0,0\r\n


进入 Deep-sleep 模式
AT+GSLP=1000\r\n


设置 sleep 模式
AT+SLEEP=0\r\n


设置 wifi 模式为 station 模式，保存到 Flash
AT+CWMODE_DEF=1\r\n
return:
AT+CWMODE_DEF=1\r\n
\r\nOK\r\n


连接wifi保存到flash：
AT+CWJAP_DEF="topwisesz","123456789"\r\n
return:
AT+CWJAP_DEF="topwisesz","123456789"\r\n
WIFI CONNECTED\r\n
WIFI GOT IP\r\n
\r\nOK\r\n


连接wifi不保存到flash:
AT+CWJAP_CUR="topwisesz","123456789"\r\n


查询 ESP8266 Station 已连接的 AP 信息
AT+CWJAP_DEF?\r\n
return:


设置搜索wifi CWLAP 指令的属性
AT+CWLAPOPT=1,2047\r\n


搜索wifi
AT+CWLAP\r\n


断开wifi
AT+CWQAP\r\n


上电是否自自动连接 AP，保存到 Flash，ESP8266 Station 默认上电自自动连接 AP。
AT+CWAUTOCONN=0\r\n
return:\r\nOK\r\n


查询 ESP8266 Station 的 IP 地址，必须连上AP后，才可以查询
AT+CIPSTA_DEF?\r\n
返回：
+CIPSTA_DEF:<ip>
+CIPSTA_DEF:<gateway>
+CIPSTA_DEF:<netmask>
OK


查询 ESP8266 Station 的主机名称
AT+CWHOSTNAME?\r\n
返回:
AT+CWHOSTNAME\?\r\n
+CWHOSTNAME:ESP_2C2CCB\r\n
\r\nOK\r\n


查询 ESP8266 WiFi 国家码
AT+CWCOUNTRY_DEF?\r\n
返回:
AT+CWCOUNTRY_DEF\?\r\n
+CWCOUNTRY_DEF:0,"CN",1,13\r\n
\r\nOK\r\n


查询本地 IP 地址
AT+CIFSR\r\n
返回：
+CIFSR:APIP,<SoftAP IP	address>
+CIFSR:APMAC,<SoftAP	MAC address>
+CIFSR:STAIP,<Station IP address>
+CIFSR:STAMAC,<Station MAC address>
OK


Ping 功能
AT+PING="www.baidu.com"\r\n
返回：
+<time>
OK

或者：
+timeout
ERROR


设置时区和 SNTP 服务器
AT+CIPSNTPCFG=1,8\r\n


查询 SNTP 时间
AT+CIPSNTPTIME?\r\n
返回：
+CIPSNTPTIME:<time>
OK



/****************************************************************************************************************************************************
***
****************************************************************************************************************************************************/
ESP8266硬件接口：
GPIO0:
UART 下载:外部拉低。
Flash 启动:悬空或外部拉高。

GPIO2：U1TXD，悬空(内部有上拉)或外部拉高。
GPIO13：U0CTS（MTCK）
GPIO15：U0RTS（MTDO），外部需拉低。
GPIO3：U0RXD
GPIO1：U0TXD


1. 串口
UART0: U0TXD（悬空或外部拉高）, U0RXD, U0RTS, U0CTS
UART1: U1TXD

下载: U0TXD + U0RXD 或者 U1TXD + U0RXD
通信 (UART0): U0TXD, U0RXD, U0RTS, U0CTS
调试: U1TXD 可作为调试信息的打印。

UART0 在 ESP8266EX 上电时默认会输出一一些打印信息。对此敏感的应用,可以使用 UART 的内部引脚交换功能。
在初始化的时候, 将 U0CTS 作为 U0RXD，U0RTS 作为 U0TXD
硬件上的下载管脚还是使用 U0TXD + U0RXD
硬件上也从 U0RTS 和 U0CTS 连出 UART0, 连接到对应的外部 MCU 的串口进行通信。

文档：
esp-wroom-02d_esp-wroom-02u_技术规格书_cn.pdf


/*连接方式：
ESP8266的3.3V和EN，连接到MT6260的3.3V供电。
ESP8266的U0TXD，连接到MT6260的RX。
ESP8266的U0RXD，连接到MT6260的TX。
ESP8266的U0RTS，拉低
ESP8266的U0CTS，悬空

MT6260不需要开机
下载模式：
GPIO0：拉低

启动模式：
GPIO0：拉高或者悬空。*/

编译AT：
make COMPILE=gcc BOOT=new APP=1 SPI_SPEED=40 SPI_MODE=QIO SPI_SIZE_MAP=5
COMPILE=gcc
BOOT=1(new)
APP=1
SPI_SPEED=40
SPI_MODE=QIO
SPI_SIZE_MAP=5

blank.bin                   			0x1FB000
esp_init_data_default_v08.bin   	0x1FC000
blank.bin					0xFE000
blank.bin					0x1FE000
boot_v1.7.bin              			0x00000
user1.2048.new.5.bin        		0x01000


MT6260不需要开机
下载模式：
ESP8266的3.3V和EN，连接到MT6260的3.3V供电。
ESP8266的 U0TXD，连接到串口下载线 RX。
ESP8266的 U0RXD，连接到串口下载线 TX。
ESP8266的 U0RTS，拉低
ESP8266的 U0CTS，悬空
GPIO0：拉低

下载工具选择：
CrystalFreq: 26M
SPI SPEED: 40MHz
SPI MODE: QIO
FLASH SIZE: 16Mbit-C1
COM: COM5
BAUD: 115200








