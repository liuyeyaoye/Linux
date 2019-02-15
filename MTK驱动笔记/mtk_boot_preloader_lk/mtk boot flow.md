

```c
一． MTK
MTK芯片内部有一个BootRom和一个ISRAM
启动流程为：BootRom ，preloader，   lk，kernel，Android

二．手机内存的概念：
1. 运行内存RAM(最高8G)
RAM的特点：掉电数据丢失
发展过程：SDRAM ，  DDR  ，LPDDR

2. ROM(最高256G)
存储手机的数据，掉电数据不会丢失，手机的preloader,lk,bootimage,systemimage都是存储在ROM中。
发展过程：Nand  Flash  ，  Nor Flash， eMMC（Nand集成芯片），UFS

三． MTK启动流程：
1. 手机上电后，运行bootROM中的程序，bootROM初始化eMMC，然后将 eMMC中的 preloader加载到MTK的内部 ISRAM 中，然后运行preloader.
2. Preloader首先初始化DDR，然后将 eMMC中的 lk 加载到 DDR中，然后跳到 DDR 中执行 lk.
3. lk 将eMMC 中的 bootimage（Linux kernel和 RamDisk根文件系统）加载到 DDR中，执行kernel.
4. Kernel启动init进程，启动Android


四． 具体流程
1. BootROM的主要工作：
       ARM Cores execute from RESET vector；
       初始化芯片内部SRAM的stack；
       找到和初始化启动媒介（例如eMMC）；
       从启动媒介加载preloader到内部SRAM；
       跳到preloader执行。

2. preloader的主要工作：
       初始化C环境：初始化寄存器，初始化堆栈和BSS段，禁止IRQ
       初始化关键硬件：初始化Timer, clocks, UART, EMI for external DRAM
       Security setup：Only loads MTK-certified binary
       跳到  lk

Preloader相关的代码：
idh.code/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/init/ init.s
idh.code/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/core/main.c   partition.c
idh.code/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/drivers/platform.c







3. lk 的主要工作：
解析从preloader传过来的参数；
使能MMU cache
使能外设；
设置启动模式；
加载kernel；
跳到kernel执行：调用   lk/app/mt_boot/mt_boot.c /Boot_linux_from_storage函数，将跳转到Linux Kernel，不再返回。

LK必须以ATAG的结构向Kernel 传参。
ATAG必须以ATAG_CORE开始ATAG_NONE结束（几个Bank几个ATAG_MEM）。
进入Kernel 前必须关闭DCACHE和禁用MMU，然后跳到kernel。
PC指向CFG_BOOTIMG_LOAD_ADD即Kernel所在地址，并将machtype和tags地址分别存入R1和R2，R0为0。

Lk相关的代码：
idh.code/vendor/mediatek/proprietary/bootable/bootloader/lk/arch/arm/crt0.s
vendor/mediatek/proprietary/bootable/bootloader/lk/kernel/main.c    thread.c
vendor/mediatek/proprietary/bootable/bootloader/lk/platform/mt6735/platform.c
vendor/mediatek/proprietary/bootable/bootloader/lk/app/app.c
vendor/mediatek/proprietary/bootable/bootloader/lk/app/mt_boot/mt_boot.c






preloader到lk的参数传递：
preloader的参数：
vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/drivers/inc/platform.h     有结构体boot_arg_t，
vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/drivers/platform.c     对参数赋值
lk的参数：
vendor/mediatek/proprietary/bootable/bootloader/lk/platform/mt6735/include/platform/boot_mode.h   有结构体BOOT_ARGUMENT
vendor/mediatek/proprietary/bootable/bootloader/lk/kernel/main.c  中的kamin函数来读取参数值

4. Linux kernel初始化
Entry point----stext
	……
调用__create_page_tables 子函数来建立一级页表，因为内核代码用的都是虚拟地址，在使用之前我们必须要建立MMU。这里的MMU只需要建立的页表能识别内核代码这部分的虚拟地址就够了。主页表将4 GB 的地址空间分成若干个1 MB的段(section)。每个页表项是32 bits(4 bytes)，因而主页表占用
 4096 *4 = 16k的内存空间。
……
Start Kernel
Boot param
Kthread
Kernel_init()
Init.rc		根据启动参数执行android的首个进程init

相关的代码：
idh.code/kernel-3.18/arch/arm/kernel/head.s  head-common.s
5. UART配置信息在哪里配置?
idh.code/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6735/src/drivers/platform.c  中的CFG_UART_LOG 宏
vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/top6737t_36_a_n/cust_bldr.mak 有该宏定义

6.为何ENG版本显示首个启劢logo的左下角的bootmode, USER不显示？
vendor/mediatek/proprietary/bootable/bootloader/lk/platform/mt6735/platform.c中
有宏NORMAL_BOOT


程序内存知识：
BSS段：用来存放程序中未初始化的全局变量的一块内存区域，属于静态内存分配。
数据段：用来存放程序中已初始化的全局变量的一块内存区域。属于静态内存分配。
代码段：用来存放程序执行代码的一块内存区域，这部分区域的大小在程序运行前就已经确定，并且内存区域通常属于只读, 某些架构也允许代码段为可写，在代码段中，也有可能包含一些只读的常数变量，例如字符串常量等。
堆（heap）：堆是用于存放进程运行中被动态分配的内存段，它的大小并不固定，可动态扩张或缩减。当进程调用malloc等函数分配内存时，新分配的内存就被动态添加到堆上（堆被扩张）；当利用free等函数释放内存时，被释放的内存从堆中被剔除（堆被缩减）
栈(stack)：栈又称堆栈， 是用户存放程序临时创建的局部变量，也就是说我们函数括弧“{}”中定义的变量（但不包括static声明的变量，static意味着在数据段中存放变量）。除此以外，在函数被调用时，其参数也会被压入发起调用的进程栈中，并且待到调用结束后，函数的返回值也会被存放回栈中。由于栈的先进先出特点，所以栈特别方便用来保存/恢复调用现场。
栈（stack）和堆（heap）的区别：
栈（stack）由编译器自动分配释放 ，存放函数的参数值，局部变量的值等
堆（heap） 一般由程序员分配释放， 若程序员不释放，程序结束时可能由OS回收
```


