
```c
MKT平台移植LCD驱动：

一、配置文件
1、阅读硬件原理图和GPIO配置表，确定LCD的引脚配置是否正确，以xxx项目为例：
MIPI引脚(GPI150—GPI159)：TCP/TCN，TDP0/TDN0，TDP1/TDN1，TDP2/TDN2，TDP3/TDN3
DSI_TE(GPIO147) ， 
LCM_RST(GPIO146) ， 
LCM_ID(GPIO122) ,
LCM_LEDA/LCM_LEDK(背光控制引脚，与背光芯片AW9961 相连)：GPIO69为背光控制引脚。
VIO18 ， VIO28

2、配置 dws 文件(dws文件有4个路径：preloader、lk、kernel、custom)：
DSI_TE引脚，GPIO147：				IN
LCM复位引脚，GPIO146：				OUT
LCM ID检测引脚，GPIO122：			IN
LCM背光控制引脚，GPIO69：			OUT
LCM MIPI引脚，GPI150—GPI159：		MIPI

3、配置 dts 文件：$(project).dts
配置背光引脚 GPIO69 的 output-low 和 output-high 状态。当backlight的工作模式为 GPIO 时会用到，其他地方用不到。

**********关注一下MTK的背光几种工作模式(一般采用第二种)：
a、由PWM来控制 backlight ：
(1)PWM：		直接采用BB端的PWM模块输出，			配置为： MT65XX_LED_MODE_PWM
(2)AAL(BLS)：		经过BB端的BLS模块后，再由PWM模块输出，	配置为： MT65XX_LED_MODE_CUST_BLS_PWM
(3)GPIO：		直接通过GPIO69来输出PWM波形，		配置为： MT65XX_LED_MODE_GPIO
(4)CABC：		通过MIPI写LCM IC端寄存器，由backlight IC 输出PWM波形(LCM中必须有背光IC)，	配置为： MT65XX_LED_MODE_CUST_LCM

b、由PMIC控制backlight，  配置为 MT65XX_LED_MODE_PMIC

c、lk中在 cust_led_list 这个数组中配置背光模式为MT65XX_LED_MODE_CUST_BLS_PWM 。kernel中在dts中配置 led_mode = <5> 即可生效。

**********关注一下AAL和CABC这两个名词：
a、AAL有两种含义：一是表示AAL这种backlight模式(即BLS)；二是表示AALServer

b、CABC也有两种含义：一是表示CABC这种backlight模式(直接向LCM backlight IC写寄存器)；二是表示AALServer下的一个子feature。
												
c、AALServer(MTK特有的一种技术)包括三个子feature：
(1)LABC：根据环境调节backlight(从L版本之后使用Android原生的自动背光功能)
(2)CABC：根据Context调节背光(省电)
(3)DRE：增强阳光下的可读性(与L sensor相关，与backlight无关)


二、移植驱动
1、	直接使用通用屏驱动，添加hwinfo；
		或者使用屏驱动文件，修改list、Makefile等文件，添加屏驱动。
		
2、查看初始化代码中的读LCD ID的地址，从串口log中看到读到的LCD ID ，填入初始化代码。

3、LCD进睡眠写LCD的寄存器0x28和0x10，LCD唤醒直接跑初始化代码。

4、关注LCD的 esd check的地址和value。

5、关注LCD的 PCLK和 RGB timing ，PCLK影响天线干扰。也影响FPS。fps计算公式为：
(1)DSI VDO mode：
数据速率：data_rate = (height+VSA+VBP+VFP) * (width+HSA+HBP+HFP) * bpp * fps / (data_lane_num)
fps=60, RGB888的bpp=24， data_lane_num=4，则data_rate=453MHz

(2)DSI CMD mode:
数据速率： data_rate = width*height*1.2*bpp*fps/data_lane_num

(3)DSI采用双边采样(上升沿和下降沿采样数据)，时钟速率 PCLK=data_rate/2
PCLK 即为 hwinfo 中配置的 PCLK。

```


