

```c
分析 mtk lk 源代码



〇.相关的结构体
1.与线程相关的结构体
struct list_node {
	struct list_node *prev;
	struct list_node *next;
};  /* 双向链表结构体*/

typedef struct thread {
	int magic;
	struct list_node thread_list_node;

	/* active bits */
	struct list_node queue_node;
	int priority;
	enum thread_state state;	
	int saved_critical_section_count;
	int remaining_quantum;

	/* if blocked, a pointer to the wait queue */
	struct wait_queue *blocking_wait_queue;
	status_t wait_queue_block_ret;

	/* architecture stuff */
	struct arch_thread arch;

	/* stack stuff */
	void *stack;
	size_t stack_size;

	/* entry point */
	thread_start_routine entry;
	void *arg;

	/* return code */
	int retcode;

	/* thread local storage */
	uint32_t tls[MAX_TLS_ENTRY];

	char name[32];
} thread_t; /* 线程数据结构 */

2. BOOT_ARGUMENT 数据结构
typedef struct {
  u32      maggic_number;
  BOOTMODE boot_mode;
  u32      e_flag;
  u32      log_port;
  u32      log_baudrate;
  u8       log_enable;
  u8       part_num;
  u8       reserved[2];
  u32      dram_rank_num;
  u32      dram_rank_size[4];
  mblock_info_t mblock_info;	/* number of mblock should be max rank number + 1 */
  dram_info_t orig_dram_info;
  mem_desc_t lca_reserved_mem;
  mem_desc_t tee_reserved_mem;
  u32      boot_reason;
  META_COM_TYPE meta_com_type;
  u32      meta_com_id;
  u32      boot_time;
  da_info_t da_info;
  SEC_LIMIT sec_limit;
  part_hdr_t *part_info;
  u8 md_type[4];
  u32  ddr_reserve_enable;
  u32  ddr_reserve_success;
  u32  ddr_reserve_ready;
  ptp_info_t ptp_volt_info;
  u32  dram_buf_size;
  u32  meta_uart_port;
  u32  smc_boot_opt;
  u32  lk_boot_opt;
  u32  kernel_boot_opt;
  u32 non_secure_sram_addr;
  u32 non_secure_sram_size;
  u32 dtb_addr;
  u32 dtb_size;
  char pl_version[8];
} BOOT_ARGUMENT;

3.struct boot_img_hdr 数据结构
struct boot_img_hdr
{
    unsigned char magic[BOOT_MAGIC_SIZE];

    unsigned kernel_size;  /* size in bytes */
    unsigned kernel_addr;  /* physical load addr */

    unsigned ramdisk_size; /* size in bytes */
    unsigned ramdisk_addr; /* physical load addr */

    unsigned second_size;  /* size in bytes */
    unsigned second_addr;  /* physical load addr */

    unsigned tags_addr;    /* physical addr for kernel tags */
    unsigned page_size;    /* flash page size we assume */
    unsigned unused[2];    /* future expansion: should be 0 */

    unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */
    
    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */
};




一.分析 kmain 函数
void kmain(void)
{
	thread_init_early();
/* 线程的早起初始化，填充一个名为"bootstrap"的线程的结构体，并将该线程添加到线程表 thread_list。
注意，此时还没有 create 这个线程，仅仅只初始化它的数据结构。
设置了 current_thread 为 bootstrap_thread 这个线程。
 */	

	arch_early_init();
/* armV7架构相关的初始化。先禁止 cache。设置 vector 地址。初始化 mmu map。然后使能 cache。 */

	platform_early_init();
/* 平台早期的硬件初始化。在序号二分析该函数。*/
	
	call_constructors();
/* 该函数会调用 system-onesegment.ld 中地址为 __ctor_list 和 __ctor_end 之间的 .ctors 的内容 */	

	heap_init();
/* 初始化堆：初始化它的开始地址和长度。
堆与malloc函数相关，而栈是由操作系统分析的，如函数的局部变量等。*/

	dpc_init();
/*dpc 初始化，主要的功能有：初始化一个 dpc event。创建 dpc 线程。
thread_resume(thread_create("dpc", &dpc_thread_routine, NULL, DPC_PRIORITY, DEFAULT_STACK_SIZE));
使用thread_create函数创建的线程，其状态为THREAD_SUSPENDED，线程执行函数为 dpc_thread_routine。
使用thread_resume函数，使得线程状态为THREAD_READY。
事实上， thread_create 函数创建一个挂起的线程， thread_resume 函数唤醒这个挂起的线程。
唤醒后，开始执行线程函数，线程函数执行完后，返回这里。所以在 lk 中是单线程执行的。
*/

	timer_init();
/* 初始化内核定时器：定义超时回调函数。 */

	thread_resume(thread_create("bootstrap2", &bootstrap2, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE));
/*创建一个 bootstrap2 的线程，线程函数的入口地址为 bootstrap2 。
序号三分析线程函数 bootstrap2。
*/
	
	exit_critical_section();
/* 该函数的功能是离开临界区：使能中断*/	

	thread_become_idle();
/*设置当前进程为 idle 空闲进程*/
}



二.分析 platform_early_init 函数
void platform_early_init(void)
{
	unsigned int time_wdt_early_init;
	unsigned int time_pmic_init;
	unsigned int time_platform_early_init;

	platform_init_interrupts();
/* 初始化中断：配置与中断相关的寄存器。 */

	platform_early_init_timer();
/* 初始化定时器：配置与定时器相关的寄存器。 */

	mt_gpio_set_default();
/* 与 GPIO 相关的寄存器配置。 */

	uart_init_early();
/* 初始化 uart */

    hwinfo_init(HWINFO_ADDR);
/* 判断 hwinfo 的地址的 HWINFO_MAGIC 是否和 preloader 传来的地址匹配。获取 hwinfo 地址存储的数据的 size 。
magic 是一个地址标志，放在地址的首字节，代表该地址的标志。 */

	mtk_wdt_init();
/* 初始化 wdt：配置与看门狗相关的寄存器。 */

#ifdef CONFIG_BSP_HWINFO
	int power_on_vibrator = 0;
	if (hwinfo_parser_fetch("vibrator", "power_on_vibrator", &(power_on_vibrator), 1) <= 0) {
    vibr_Enable_HW();
	} 
	else if (power_on_vibrator != 0){
		vibr_Enable_HW();
	} 
	else {
        ;
	}
/* 使能开机振动器 */
#endif

	pmic_init();
/* 该函数实际上什么也没做， pmic 已经在 preloader 初始化了 ??? */
}



三.分析 bootstrap2 函数
static int bootstrap2(void *arg)
{
	platform_init();
/*平台初始化函数。
序号四详细分析该函数*/

	apps_init();
/*序号五分析该函数*/
	
	return 0;
}




四.分析 platform_init 函数
void platform_init(void)
{
	u64 pl_start_addr = 0;
	plinfo_get_brom_header_block_size(&pl_start_addr);
/*该函数功能，使 pl_start_addr= EMMC_HDR_BLK_SIZE 。
获取 preloader image 的 offset in preloader partition
 */

	time_nand_emmc = get_timer(0);
/*初始化 nand/emmc 计时开始。使用 get_timer(0)来初始化计时时间。*/

	mmc_legacy_init(1);
/* 初始化 emmc 。主要做的事情有：填充 struct mmc_host 、 struct mmc_card 、 block_dev_desc_t 、 part_dev_t  
这几个与emmc块设备相关的结构体，然后调用函数 mt_part_register_device 注册一个 part_dev_t 结构体。
并且打印出串口信息：[SD0] boot device found */

	dprintf(INFO, "[PROFILE] ------- NAND/EMMC init takes %d ms -------- \n", (int)get_timer(time_nand_emmc));
/* 输出 emmc 初始化耗费的时间，使用 get_timer(time_nand_emmc) 来获取时间。*/
	
	if (bldr_load_dtb("boot")<0)
		dprintf(CRITICAL, "bldr_load_dtb fail\n");
/*加载dtb到DRAM。*/

	leds_init();
/*初始化led灯和背光*/

#ifdef CONFIG_BSP_HWINFO
	int power_on_vibrator = 0;

	if (hwinfo_parser_fetch("vibrator", "power_on_vibrator", &(power_on_vibrator), 1) <= 0) {
		vibr_Disable_HW();
	} 
	else if (power_on_vibrator != 0){
		vibr_Disable_HW();
	} 
	else {
        ;
	}
/*关闭振荡器*/
#endif

	env_init();
	print_env();
/*env初始化，并且打印出env。串口输出log为：
[LK_ENV]ENV SIG Wrong
[LK_ENV]no valid env
*/
	
	g_fb_size = mt_disp_get_vram_size();
/* 显示驱动在这里初始化，第一次会调用：
pgc->plcm = disp_lcm_probe(NULL, LCM_INTERFACE_NOTDEFINED);
来初始化LCM驱动
*/
	g_fb_base = mblock_reserve(&g_boot_arg->mblock_info, g_fb_size, 0x10000, 0x100000000, RANKMAX);
	mt_disp_init((void *)g_fb_base);
	mt_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
	mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
/*这些函数都涉及disp驱动的初始化，在下一章详细分析*/

	sec_func_init(pl_start_addr);
	seclib_set_oemkey(g_oemkey, OEM_PUBK_SZ);
/* 这2个函数与这个文件 platform/mt6735/lib/libsec.a 相关。功能是初始化security library*/

	drv_video_init();
/*video 驱动初始化。配置CFB_888RGB_24BIT等。与显示相关的设置。*/

	boot_mode_select();
/*选择启动模式为NORMAL_BOOT，还有F_FACTORY_MODE、DOWNLOAD_BOOT或者RECOVERY_BOOT*/

	mboot_common_load_logo((unsigned long)mt_get_logo_db_addr_pa(), "logo");
/*加载logo。该函数实际调用 mboot_common_load_part("logo", logo_addr);来加载logo到指定地址。*/	
	
	mt65xx_bat_init();
/*初始化电池相关。检测是否低电量。*/	

	mt65xx_backlight_on();
/*开启背光*/

	mt_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
/*刷新显示*/

	sw_env();
/*	该函数中有：video_printf(" => NORMAL BOOT\n");
会在手机界面显示这个字符串。*/
}



五. 分析 apps_init 函数
void apps_init(void)
{
	const struct app_descriptor *app;

	/* call all the init routines */
	for (app = &__apps_start; app != &__apps_end; app++) {
		if (app->init)
			app->init(app);
	}

	/* start any that want to start on boot */
	for (app = &__apps_start; app != &__apps_end; app++) {
		if (app->entry && (app->flags & APP_FLAG_DONT_START_ON_BOOT) == 0) {
			start_app(app);
		}
	}
/*在 mt_boot.c 中，有 
APP_START(mt_boot)
.init = mt_boot_init,
 APP_END
 mt_boot_init 函数实际上调用了 boot_linux_from_storage 函数。
 序号六分析 boot_linux_from_storage 函数
 */
}



六. 分析 boot_linux_from_storage 函数
int boot_linux_from_storage(void)
{
	ret = mboot_android_load_bootimg_hdr("boot", CFG_BOOTIMG_LOAD_ADDR);
/*加载boot image header到DRAM。*/

	kimg_load_addr = (g_boot_hdr!=NULL) ? g_boot_hdr->kernel_addr : CFG_BOOTIMG_LOAD_ADDR;

	ret = mboot_android_load_bootimg("boot", kimg_load_addr);
/*加载 bootimg*/

	memcpy((char *)g_boot_hdr->ramdisk_addr, (char *)(g_rmem_off), g_rimg_sz);
	g_rmem_off =  g_boot_hdr->ramdisk_addr;
/*重定向 rootfs*/

	custom_port_in_kernel(g_boot_mode, cmdline_get());
/*修改cmdline的串口号，修改后的cmdline为：
"console=tty0 console=ttyMT0,921600n1 root=/dev/ram vmalloc=496M androidboot.hardware=mt6735 slub_max_order=0 slub_debug=O"
*/
	cmdline_append(cmdline_tmpbuf);
/*追加cmdline*/

	boot_linux((void *)g_boot_hdr->kernel_addr, (unsigned *)g_boot_hdr->tags_addr, (char *)cmdline_get(), 
		board_machtype(), (void *)g_boot_hdr->ramdisk_addr, g_rimg_sz);
/*从lk传入kernel的值的含义分别为： 
g_boot_hdr->kernel_addr：kernel地址
g_boot_hdr->tags_addr：   tag地址
cmdline_get()：					cmdline
board_machtype()：			MACH_TYPE
g_boot_hdr->ramdisk_addr：ramdisk地址
g_rimg_sz：						ramdisk大小
序号七分析 boot_linux 函数
*/
		
	while (1) ;
	return 0;
}



七. 分析 boot_linux 函数
void boot_linux(void *kernel, unsigned *tags,char *cmdline, unsigned machtype,void *ramdisk, unsigned ramdisk_size)
{
#ifdef DEVICE_TREE_SUPPORT
	boot_linux_fdt((void *)kernel, (unsigned *)tags, (char *)cmdline, machtype,(void *)ramdisk, ramdisk_size);
/*序号八分析 boot_linux_fdt 函数*/
	
	while (1) ;
#endif
}



八. 分析 boot_linux_fdt 函数
int boot_linux_fdt(void *kernel, unsigned *tags,char *cmdline, unsigned machtype,void *ramdisk, unsigned ramdisk_size)
{
	void *fdt = tags;
	void (*entry)(unsigned,unsigned,unsigned*) = kernel;

	zimage_size = *(unsigned int *)((unsigned int)kernel+0x2c) - *(unsigned int *)((unsigned int)kernel+0x28);
	dtb_addr = (unsigned int)kernel + zimage_size;
	dtb_size = fdt32_to_cpu(*(unsigned int *)(dtb_addr+0x4));
/*zimage的大小和dtb的地址和大小*/
	
	target_fdt_model(fdt);

	load_modem_image();

	ret = setup_mem_property_use_mblock_info(&mem_reg_property[0],sizeof(mem_reg_property)/sizeof(dt_dram_info));

	ret = target_fdt_dram_dummy_read(fdt, g_nr_bank);
	
	offset = fdt_path_offset(fdt, "/memory");

	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property, ((int)get_mblock_num? get_mblock_num(): g_nr_bank ) * sizeof(dt_dram_info));
	
	ret = platform_atag_append(fdt);

	ret = fdt_memory_append(fdt);

	ret = trusty_dts_append(fdt);

	offset = fdt_path_offset(fdt, "/chosen");

	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-start",(unsigned int) ramdisk);
	
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-end", (unsigned int)ramdisk + ramdisk_size);
	
	ret = fdt_setprop(fdt, offset, "atag,boot", buf, ptr - buf);

	ret = fdt_setprop(fdt, offset, "atag,imix_r", buf, ptr - buf);

	ptr = (char *)target_atag_mem((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);

	ptr = (char *)target_atag_partition_data((unsigned *)buf);	
	ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);

	ptr = (char *)target_atag_nand_data((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);

	ptr = (char *)target_atag_devinfo_data((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,devinfo", buf, ptr - buf);
	……
/*上面的函数作用：解析dtb，写入到DRAM指定区域*/	

	sprintf(cmdline,"%s%s",cmdline," printk.disable_uart=0 ddebug_query=\"file *mediatek* +p ; file *gpu* =_\"");
	
	hwinfo_add_cmdline(cmdline);
	sprintf(cmdline, "%s%s%d", cmdline, " boot_reason=", boot_reason);

	entry(0, machtype, tags);
/*跳转到kernel地址，执行kernel。传入的参数为 MACH_TYPE和tags的地址*/	

	while (1);
	return 0;	
}


************************************************************************************************************************************************
深入分析fdt的传递过程
一. 分析 mt_boot_init
1.mt_boot_init 函数体
void mt_boot_init(const struct app_descriptor *app)
	boot_linux_from_storage();
		
2.boot_linux_from_storage 函数体
int boot_linux_from_storage(void)
{
	ret = mboot_android_load_bootimg_hdr("boot", CFG_BOOTIMG_LOAD_ADDR);
	kimg_load_addr = CFG_BOOTIMG_LOAD_ADDR;
	ret = mboot_android_load_bootimg("boot", kimg_load_addr);
	memcpy((char *)CFG_RAMDISK_LOAD_ADDR, (char *)(g_rmem_off), g_rimg_sz);
	g_rmem_off =CFG_RAMDISK_LOAD_ADDR;

	boot_linux((void *)CFG_BOOTIMG_LOAD_ADDR, (unsigned *)CFG_BOOTARGS_ADDR,
  (char *)g_cmdline, 6735, (void *)CFG_RAMDISK_LOAD_ADDR, g_rimg_sz);


}

#define DRAM_PHY_ADDR   0x40000000
#define CFG_RAMDISK_LOAD_ADDR           (DRAM_PHY_ADDR + 0x4000000)
#define CFG_BOOTIMG_LOAD_ADDR           (DRAM_PHY_ADDR + 0x8000)
#define CFG_BOOTARGS_ADDR               (DRAM_PHY_ADDR + 0x100)


int boot_linux_fdt(void *kernel, unsigned *tags, char *cmdline, unsigned machtype, void *ramdisk, unsigned ramdisk_size)
{
	void *fdt = CFG_BOOTARGS_ADDR;
	void (*entry)(unsigned,unsigned,unsigned*) = CFG_BOOTIMG_LOAD_ADDR;
	zimage_size = *(unsigned int *)((unsigned int)kernel+0x2c) - *(unsigned int *)((unsigned int)kernel+0x28);
	dtb_addr = (unsigned int)kernel + zimage_size;
	
	if (fdt32_to_cpu(*(unsigned int *)dtb_addr) == FDT_MAGIC) 
		dtb_size = fdt32_to_cpu(*(unsigned int *)(dtb_addr+0x4));
	
	strcpy(&buf[FDT_BUFF_SIZE], "BUFFEND");

	g_CMDLINE = cmdline;
	
	target_fdt_model(fdt);
/*设置：
[7160] model=MT6737
*/	

	dt_dram_info mem_reg_property[128];
	ret = setup_mem_property_use_mblock_info(&mem_reg_property[0], 128);
	
	g_nr_bank = 3;
	ret = target_fdt_dram_dummy_read(fdt, g_nr_bank);
/*跟dtsi文件的 reserved-memory 相关*/	
	
/* Get offset of the memory node */	
	offset = fdt_path_offset(fdt, "/memory");
	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property,  3* sizeof(dt_dram_info));
	
	ret = platform_atag_append(fdt);
	
/* Get offset of the chosen node */	
	offset = fdt_path_offset(fdt, "/chosen");
	
/* Adding the linux,initrd-start to the chosen node */	
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-start",(unsigned int) ramdisk);
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-end", (unsigned int)ramdisk + ramdisk_size);
	
/*在 fdt 中增加一个属性名为 "atag,boot" 的节点，存储它的内容*/	
	ptr = (char *)target_atag_boot((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,boot", buf, ptr - buf);
	
	ptr = (char *)target_atag_imix_r((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,imix_r", buf, ptr - buf);

	ptr = (char *)target_atag_mem((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);
	
	ptr = (char *)target_atag_partition_data((unsigned *)buf);
		if (ptr != buf) 
			ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);
	
	ptr = (char *)target_atag_devinfo_data((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,devinfo", buf, ptr - buf);

	ptr = (char *)target_atag_videolfb((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,videolfb", buf, ptr - buf);
	
	ptr = (char *)target_atag_mdinfo((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,mdinfo", buf, ptr - buf);
	
	ptr = (char *)target_atag_ptp((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,ptp", buf, ptr - buf);

	ptr = (char *)target_atag_masp_data((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,masp", buf, ptr - buf);

	sprintf(cmdline,"%s%s",cmdline," printk.disable_uart=0 ddebug_query=\"file *mediatek* +p ; file *gpu* =_\"");
	
	hwinfo_add_cmdline(cmdline);
	
	pl_t = g_boot_arg->boot_time;
	sprintf(cmdline, "%s%s%d", cmdline, " bootprof.pl_t=", pl_t);
	
	lk_t = ((unsigned int)get_timer(boot_time));
	sprintf(cmdline, "%s%s%d", cmdline, " bootprof.lk_t=", lk_t);
	
	boot_reason = g_boot_arg->boot_reason;
	sprintf(cmdline, "%s%s%d", cmdline, " boot_reason=", boot_reason);
	
	sprintf(cmdline,"%s%s%s",cmdline," audio_verno=", AUDIO_VERNO);
	
	entry(0, machtype, tags);
/*tags 即 fdt*/	
}

```
