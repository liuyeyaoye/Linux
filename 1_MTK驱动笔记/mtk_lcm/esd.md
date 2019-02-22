


```c

一．问题：机器不能通过ESD检测

二．解决方案：在进行ESD检测时，将LCM切换到page 0F，以通过ESD检测。

ESD检测：让LCM亮屏，使用+10V和-10V电压（空气正负10Kv，接触正负6Kv，200次），一直打静电到它灭屏。

含义：
在初始化代码中完成对LCM的寄存器的配置后，进入LCM的page 0F（无效页）。
当进行ESD检测时，先切换到LCM的page 0，读取寄存器0x0A的值，然后再切换到page 0F。
进休眠时，也在进行ESD检测，先切换到LCM的page 0，发送命令地址0x28（Display Off）和0x10（Sleep In），进入LCM休眠模式，再切换到page 0F。
ESD检测结束后，按开机键唤醒LCM，从lcd_init_para开始执行，不是lcd_exist_sleep。
在读写LCM的寄存器时，切换到 page 0。
在其他不需要操作LCM寄存器的时候，切换到 page 0F。

1. 在初始化的最后一行加上page 0F： {0xFF,03,{0x98,0x81,0x0F}}
含义：LCM默认工作在page 0。在LCM初始化结束后，将LCM切换到page 0F。

2. 再将ESD check的流程改为先切到page 0（{0xFF,03,{0x98,0x81,0x00}},）
去check完 0x0A后，再切到page 0F（{0xFF,03,{0x98,0x81,0x0F}},）。
即修改 ddp_dsi.c文件
含义：在做ESD检测时，将LCM切换到page 0，检测完寄存器0x0a后，再切换到page F。
修改前：
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[0], AS_UINT32(&t1));
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[1], AS_UINT32(&t0));
DSI_OUTREG32(cmdq_trigger_handle, &DSI_REG[dsi_i]->DSI_CMDQ_SIZE,2);
修改后：
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[0], 0x00043902);
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[1], 0x008198ff);
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[2], AS_UINT32(&t1));
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[3], AS_UINT32(&t0));
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[4], 0x00043902);
DSI_OUTREG32(cmdq_trigger_handle, &DSI_CMDQ_REG[dsi_i]->data[5], 0x0F8198ff);
DSI_OUTREG32(cmdq_trigger_handle, &DSI_REG[dsi_i]->DSI_CMDQ_SIZE,6);

3. 将进休眠流程改为切到page 0 下0x28 & 0x10后再切到page 0F，进休眠的时候不需要复位，其流程如下：
{0xFF,03,{0x98,0x81,0x00}},
{0x28,01,{0x00}},
Delay_ms(50);
{0x10,01,{0x00}},
Delay_ms(120);
{0xFF,03,{0x98,0x81,0x0F}},
含义：LCM进休眠时，先切换到page 0 ，控制Display Off和Sleep in，然后切换到 page 0F 
　　
三．分析和注意：
1. 在初始化的最后一行加上page 0F： {0xFF,03,{0x98,0x81,0x0F}}，即在lcd_init_para的最后一行添加：0X9004,0xFF,0x98,0x81,0x0F
注意，最后一行不能有逗号。

2. 0x00043902 的含义
参考 LCM_Porting_Guide_DSI_V1.0_MT6735(P_M)_MT6753.pdf  的DSI Command Queue（工作在 DSI CMD模式，而不是 video 模式）

最后1个字节 0x02 为 CONFG：
Bit7：RPT	
Bit6：RSV	
Bit5：TE 
Bit4：CL	
Bit3：HS	
Bit2：BTA	
[Bit1:Bit0]:  TYPE[1:0]

Bit3： 
当 HS=1，DSI传输模式为HS模式。
当 HS=0，DSI传输模式为LP模式。

[Bit1:Bit0]:  TYPE[1:0]

00：短包，读/写命令。
Bit3			bit2			bit1			bit0
Data1		Data0		Data ID		CONFG

01：长包，Frame buffer write command (from LCD)。
Bit3				bit2				bit1			bit0
Mem start 1(可选)	mem start 0		Data ID		CONFG

10：长包，通用长包写命令 (from CMDQ)
Bit3			bit2			bit1			bit0
WC 1		WC 0		Data ID		CONFG

数据：
Data3		Data2		Data1		Data0


11：短包，FB读命令。
Bit3				bit2				bit1		bit0
Mem start 1(可选)	mem start 0		Data ID	CONFG

例如：	0x00043902
02：0x10，表示 长包
39：表示 Data ID
0004：表示接下来传输4个字节的数据

0x008198ff：
0xff：命令
98 81 00 ：参数

```






