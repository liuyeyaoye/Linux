
```c
http://blog.csdn.net/zz460833359/article/details/78502948

This patch is first version of Mediatek Command Queue(CMDQ) driver. 
The CMDQ is used to help read/write registers with critical time limitation, such as updating display configuration during the vblank. 
It controls Global Command Engine (GCE) hardware to achieve this requirement.
Currently, CMDQ only supports display related hardwares, but we expect it can be extended to other hardwares for future requirements.


CMDQ用于帮助读/写有关键时间限制的寄存器，例如在vblank期间更新显示配置。
它控制全局命令引擎（GCE）硬件来实现这一要求。
目前，CMDQ仅支持与显示相关的硬件，但我们预计它可以扩展到其他硬件以满足未来的需求。

in mt6761,kernel4.9,Android8.1:
[    2.932184] <0>.(3)[1:swapper/0][CMDQ]CMDQ driver late init begin
[    2.932282] <0>.(3)[1:swapper/0][CMDQ]REC: not enough SRAM:216 to create delay:368
[    2.932285] <0>.(3)[1:swapper/0][CMDQ][DelayThread]create delay thread in sram failed free:216 sram size:368
[    2.932312] <0>.(3)[1:swapper/0][CMDQ][DelayThread]create delay thread task in DRAM task size:296 sram size:216
```

