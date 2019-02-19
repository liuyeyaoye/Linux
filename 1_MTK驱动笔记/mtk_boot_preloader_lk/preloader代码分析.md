

```c
分析 mtk preloader 源代码



〇.相关的结构体：
1.struct bldr_command_handler 结构体：
struct bldr_command_handler {
    void *priv;
    u32   attr;
    bldr_cmd_handler_t cb;  /* bldr_cmd_handler_t为一个函数指针，如下所示：*/
};
typedef bool (*bldr_cmd_handler_t)(struct bldr_command_handler *handler, struct bldr_command *cmd, struct bldr_comport *comm);

2. bl_param_t 结构体：
typedef struct {
    u32 ver;
    u32 hdr_blk_dev_addr;
    hdrblk_info_t hdr_blk_info[4];        
    bl_info_t bl_loadinfo[MAX_BL_NUM];
    u32 brom_flags;
    u32 meid[ME_IDENTITY_LEN>>2];
} bl_param_t;

3.boot_arg_t 结构体
typedef struct {
    u32 magic;
    boot_mode_t mode;
    u32 e_flag;
    u32 log_port;
    u32 log_baudrate;
    u8  log_enable;
    u8  part_num;
    u8  reserved[2];
    u32 dram_rank_num;
    u32 dram_rank_size[4];
    mblock_info_t mblock_info;	/* number of mblock should be max rank number + 1 */
    dram_info_t orig_dram_info;
    mem_desc_t lca_reserved_mem;
    mem_desc_t tee_reserved_mem;
    u32 boot_reason;
    u32 meta_com_type;
    u32 meta_com_id;
    u32 boot_time;
    da_info_t da_info;
    SEC_LIMIT sec_limit;
    part_hdr_t *part_info;
    u8  md_type[4];
    u32  ddr_reserve_enable;
    u32  ddr_reserve_success;
	ptp_info_t ptp_volt_info;
    u32  dram_buf_size;
    u32  meta_uart_port;
    u32  smc_boot_opt;
    u32  lk_boot_opt;
    u32  kernel_boot_opt;
    u32 non_secure_sram_addr;
    u32 non_secure_sram_size;
    char pl_version[8];
} boot_arg_t;


4.boot_tag 结构体
typedef struct {
    struct boot_tag_header hdr;
    union {
        struct boot_tag_boot_reason boot_reason;
        struct boot_tag_boot_mode boot_mode;
        struct boot_tag_meta_com meta_com;
        struct boot_tag_log_com log_com;
        struct boot_tag_mem mem;
        struct boot_tag_md_info md_info;
        struct boot_tag_boot_time boot_time;
        struct boot_tag_da_info da_info;
        struct boot_tag_sec_info sec_info;
        struct boot_tag_part_num part_num;
        struct boot_tag_part_info part_info;
        struct boot_tag_eflag eflag;
        struct boot_tag_ddr_reserve ddr_reserve;
        struct boot_tag_dram_buf dram_buf;
        struct boot_tag_boot_opt boot_opt;
        struct boot_tag_sram_info sram_info;
        struct boot_tag_ptp ptp_volt;
        struct boot_tag_dtb_info dtb_info;
        struct boot_tag_pl_version pl_version;
    } u;
}boot_tag;

5.dram_buf_t 结构体
typedef struct{
    part_hdr_t   part_info[PART_MAX_NUM];
    part_t  partition_info[128];
    struct part_meta_info meta_info[128];
    boot_arg_t bootarg; /* 启动参数*/
    u8 log_dram_buf[LOG_BUFFER_MAX_SIZE];
    boot_tag *boottag; /*boot args pass to LK by ATAG*/
} dram_buf_t;




一.main函数：

static bl_param_t *bldr_param = NULL;

void main(u32 *arg)
{
    struct bldr_command_handler handler;
    u32 jump_addr, jump_arg;

    /* get the bldr argument */
    bldr_param = (bl_param_t *)*arg;

    mtk_uart_init(UART_SRC_CLK_FRQ, CFG_LOG_BAUDRATE);
/*mtk串口初始化函数， 时钟为12000000，波特率为921600。
UART_SRC_CLK_FRQ 宏定义为 12000000 。CFG_LOG_BAUDRATE 在 default.mak 中定义为 921600。
在preloader中，所有在代码中未找到的宏，可以在 platform/mt6735/default.mak 中找到，例如，CFG_UART_LOG :=UART1
*/
	
    bldr_pre_process();
/* 在下面的序号二中分析 bldr_pre_process 函数 */

    handler.priv = NULL;
    handler.attr = 0;
    handler.cb   = bldr_cmd_handler;
/* bldr_cmd_handler 函数根据传入的cmd，进行相应的设置和处理*/
	
    bldr_handshake(&handler);
/* 调用usb_handshake函数，与usb握手连接，并且设置 g_meta_com_type = META_USB_COM;*/

    sec_boot_check();
/* 启动时的安全性检查 */
	 
    device_APC_dom_setup();
/* APC初始化*/
	
#if CFG_ATF_SUPPORT
    trustzone_pre_init();
/* 如果支持ATF，初始化trustzone */	
#endif

   bldr_load_images(&jump_addr)；
 /* 把lk从emmc中加载到DRAM中去，序号四分析该函数
经过该函数后，jump_addr为lk的入口地址
 */

    bldr_post_process();
/* 该函数将调用platform_post_init函数。序号五分析该函数 */	

#if CFG_ATF_SUPPORT
    trustzone_post_init();
/* 初始化trustzone(与TEE、ATF相关) */
#endif

#if CFG_BOOT_ARGUMENT_BY_ATAG
    jump_arg = (u32)&(g_dram_buf->boottag);
/* jump_arg 中保存了bootargs 参数，以ATAG的形式保存。该地址将会传给lk*/	
#endif

#if CFG_ATF_SUPPORT
    /* 64S3,32S1,32S1 (MTK_ATF_BOOT_OPTION = 0)
	 * pre-loader jump to LK directly and then LK jump to kernel directly */
    if ( BOOT_OPT_64S3 == g_smc_boot_opt &&
         BOOT_OPT_32S1 == g_lk_boot_opt &&
         BOOT_OPT_32S1 == g_kernel_boot_opt) {
        print("%s 64S3,32S1,32S1, jump to LK\n", MOD);
        bldr_jump(jump_addr, jump_arg, sizeof(boot_arg_t));
    } else {
        print("%s Others, jump to ATF\n", MOD);
        bldr_jump64(jump_addr, jump_arg, sizeof(boot_arg_t));
/* 跳到ATF执行，jump_addr 为lk的入口地址，jump_arg是ATAG形式的bootargs参数的地址
序号六分析 bldr_jump64 函数
*/		
    }
#endif
}




二.分析 bldr_pre_process 函数

1.相关的结构体：
typedef enum {
    NORMAL_BOOT         = 0,
    META_BOOT           = 1,
    RECOVERY_BOOT       = 2,
    SW_REBOOT           = 3,
    FACTORY_BOOT        = 4,
    ADVMETA_BOOT        = 5,
    ATE_FACTORY_BOOT    = 6,
    ALARM_BOOT          = 7,
    FASTBOOT            = 99,
    DOWNLOAD_BOOT       = 100,
    UNKNOWN_BOOT
} boot_mode_t;

2.bldr_pre_process函数体：

boot_mode_t g_boot_mode;

static void bldr_pre_process(void)
{
    int isLocked = 0;
	
    platform_safe_mode(1, CFG_EMERGENCY_DL_TIMEOUT_MS);
/* CFG_EMERGENCY_DL_TIMEOUT_MS=1000*5
该函数设置与wdt Reset的相关寄存器
 */
	
    platform_pre_init();
/* 序号三分析该函数 */

    g_boot_mode = NORMAL_BOOT;
    platform_init();
/* 各种硬件初始化。判断从nand还是mmc启动。初始化启动媒介mmc。保存DDR参数到mmc中(使用自己配置的或者mtk的)
初始化EMI。初始化dram buffer。初始化ram console。判断是否进入preloader的下载模式。保存dram的信息到bootarg。
 */	
	
#ifdef CONFIG_BSP_FLASH
	emi_write_ddr_para_2_block();
/* 保存DDR参数到mmc中  */
#endif

    part_init();
/*  初始化partition */	

    log_buf_ctrl(1); /* switch log buffer to dram */
/* log缓冲设置  */
}



三.  分析platform_pre_init();
void platform_pre_init(void)
{
	mtk_timer_init();
/*初始化定时器*/
	
	g_boot_time = get_timer(0);
/*初始化启动时间为0 */	
	
	mt_pll_init();
/*初始化pll*/
	
	mtk_uart_init(12000000, 921600);
/*当启动pll后，重新初始化串口时钟和波特率。配置串口后，可以在log中看到输出，而不是乱码。*/
	
	clk_buf_all_on();
/*打开所有时钟*/
	
	pwrap_init_preloader();
/*初始化pmic wrapper*/
	
	pmic_ret = pmic_init();
/*初始化pmic,该函数体中有如下代码：    
print("[pmic_init] Preloader Start,MT6328 CHIP Code = 0x%x\n", get_MT6328_PMIC_chip_version());
在串口log可以看到。
该函数还会检测电池是否存在，检测电池电压和充电电压，并输出串口log。
*/

    mt_pll_post_init();
/*after pmic_init*/

    PMIC_enable_long_press_reboot();
/* 使能长按开机键重启*/

    platform_core_handler();
/* CPU处理函数，会调用8个CPU函数(8核)*/

    ptp_init();
/* 初始化ptp*/
}



四.分析 bldr_load_images 函数
static int bldr_load_images(u32 *jump_addr)
{
    blkdev_t *bootdev;
	bootdev = blkdev_get(BOOTDEV_SDMMC);
/* 获得启动设备信息 */	

#ifdef CFG_LOAD_UBOOT
#ifdef CONFIG_BSP_HWINFO
    addr = CFG_HWINFO_MEMADDR;
    ret = bldr_load_part("HWINFO", bootdev, &addr, &size);
/* 加载HWINFO信息到DRAM*/
	if(ret == -1)
    	ret = bldr_load_part("hwinfo", bootdev, &addr, &size);
#endif

    addr = CFG_UBOOT_MEMADDR;
    ret = bldr_load_part("lk", bootdev, &addr, &size);
/* 加载lk到DRAM中*/
    if (ret)
       return ret;
    *jump_addr = addr;
/* 将跳转地址jump_addr设为lk的入口地址*/
#endif
	
#if CFG_ATF_SUPPORT
    addr = CFG_ATF_ROM_MEMADDR;
    ret = bldr_load_tee_part("tee1", bootdev, &addr, 0, &size);
/* 加载tee1到DRAM中*/

    if (ret) {
        addr = CFG_ATF_ROM_MEMADDR;
        ret = bldr_load_tee_part("tee2", bootdev, &addr, 0, &size);
/* 加载tee1失败时，会加载tee2到DRAM中*/
        if (ret)
    	    return ret;        
    }

    print("%s bldr load tee part ret=0x%x, addr=0x%x\n", MOD, ret, addr);

    addr = CFG_BOOTIMG_HEADER_MEMADDR;
    size = 0x100;
    bldr_load_bootimg_header("boot", bootdev, &addr, 0, &size);
/* 加载bootimage头到DRAM中*/

    print("%s part_load_images ret=0x%x\n", MOD, ret);
#endif
}



五.分析platform_post_init函数
void platform_post_init(void)
{
#if CFG_BATTERY_DETECT

    /* normal boot状态时，检测电池存在或否 */
    if (g_boot_mode == NORMAL_BOOT && !hw_check_battery() && usb_accessory_in()) {
        print("%s Wait for battery inserted...\n", MOD);
       
	   /* 禁止充电led灯 */
        pl_close_pre_chr_led();
		
        /* 使能强制充电模式 */
        pl_charging(1);
		
        do {
            mdelay(300);
            /* check battery exists or not */
            if (hw_check_battery())
                break;
/* 循环检测电池是否存在，当检测到插入电池后，退出循环*/

            /* kick all watchdogs */
            platform_wdt_all_kick();
        } while(1);
		
        /* 禁止强制充电模式 */
        pl_charging(0);
    }
	
    /* Disable usb download wo battery */
    pmic_disable_usbdl_wo_battery();
#endif

    platform_parse_bootopt();
/* 该函数主要设置了 smc、lk、kernel三个标志位的数组，在main函数的最后会根据这三个标志位来判断是否跳到ATF */
    
	platform_set_boot_args();
/* 该函数调用    platform_set_boot_args_by_atag(&(g_dram_buf->boottag)); 
保存bootargs参数，该参数以ATAG的形式传给lk。bootargs的定义为：
#define bootarg  g_dram_buf->bootarg
*/
}

boot_tag的定义为：
typedef struct {
    struct boot_tag_header hdr;
    union {
        struct boot_tag_boot_reason boot_reason;
        struct boot_tag_boot_mode boot_mode;
        struct boot_tag_meta_com meta_com;
        struct boot_tag_log_com log_com;
        struct boot_tag_mem mem;
        struct boot_tag_md_info md_info;
        struct boot_tag_boot_time boot_time;
        struct boot_tag_da_info da_info;
        struct boot_tag_sec_info sec_info;
        struct boot_tag_part_num part_num;
        struct boot_tag_part_info part_info;
        struct boot_tag_eflag eflag;
        struct boot_tag_ddr_reserve ddr_reserve;
        struct boot_tag_dram_buf dram_buf;
        struct boot_tag_boot_opt boot_opt;
        struct boot_tag_sram_info sram_info;
        struct boot_tag_ptp ptp_volt;
        struct boot_tag_dtb_info dtb_info;
        struct boot_tag_pl_version pl_version;
    } u;
}boot_tag;



六. 分析 bldr_jump64函数
/*  实参为： bldr_jump64(jump_addr, jump_arg, sizeof(boot_arg_t)); */
void bldr_jump64(u32 addr, u32 arg1, u32 arg2)
{
	platform_wdt_kick();
/* restart wdt */

    platform_safe_mode(0, 0);
/* 禁止 preloader安全模式*/

#if CFG_ATF_SUPPORT
    trustzone_jump(addr, arg1, arg2);
#endif
}

下面是 trustzone_jump函数：
void trustzone_jump(u32 addr, u32 arg1, u32 arg2)
{
    tee_sec_config();
/*与tee相关的配置，与secro.img相关*/
	
    jumparch64(addr, arg1, arg2, ATF_BOOT_ARG_ADDR);
/* ATF_BOOT_ARG_ADDR = 0x00101000*/
}

下面是 jumparch64 的定义，在 preloader目录的init.s文件中：
jumparch64:
    MOV r4, r1   /* ATAG形式的bootargs参数的地址 */
    MOV r5, r2   /* sizeof (boot_arg_t) */
    MOV r6, r0   /* keep LK jump addr */
    MOV r7, r3   /* r3 = TEE boot entry */
将lk的地址、bootargs参数的地址、bootargs参数的大小、TEE启动的入口地址保存在寄存器后，执行lk。
	
至此，整个preloader的流程结束。

```

