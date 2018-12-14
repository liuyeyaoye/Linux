


内核：kernel-4.9

平台：XX6761（P21）

一、 一个典型的dts定义如下（dtc反编译dtb得来，取其中一部分分析）

```c
/dts-v1/;

/ {
	model = "XX6761";
	compatible = "vendor,xx6761";
	interrupt-parent = <0x1>;
	#address-cells = <0x2>;
	#size-cells = <0x2>;

	chosen {
		bootargs = "console=tty0 console=ttyS0,921600n1root=/dev/ram initrd=0x45000000,0x1000000 loglevel=8 androidboot.selinux=permissive androidboot.hardware=xx6761 initcall_debug=1 maxcpus=1";
		kaslr-seed = <0x0 0x0>;
		linux,phandle = <0x45>;
		phandle = <0x45>;
		atag,videolfb-fb_base_h = <0x0>;
		atag,videolfb-fb_base_l = "^`P";
		atag,videolfb-islcmfound = <0x1>;
		atag,videolfb-islcm_inited = <0x0>;
		atag,videolfb-fps = <0x1770>;
		atag,videolfb-vramSize = <0x17bb000>;
		atag,videolfb-lcmname = "nt35521_hd_dsi_vdo_truly_rt5081_drv";
	};

	pmu {
		compatible = "arm,armv8-pmuv3";
		interrupt-parent = <0xd>;
		interrupts = <0x1 0x7 0x8>;
	};

	utos {
		compatible = "microtrust,utos";
		interrupts = <0x0 0xbc 0x1 0x0 0xbd 0x1>;
	};
	
	interrupt-controller@0c000000 {
		compatible = "arm,gic-v3";
		#interrupt-cells = <0x3>;
		#address-cells = <0x2>;
		#size-cells = <0x2>;
		#redistributor-regions = <0x1>;
		interrupt-parent = <0xd>;
		interrupt-controller;
		reg = <0x0 0xc000000 0x0 0x40000 0x0 0xc080000 0x0 0x200000 0x0 0x10200a80 0x0 0x50>;
		interrupts = <0x1 0x9 0x4>;
		linux,phandle = <0xd>;
		phandle = <0xd>;
	};

	intpol-controller@10200a80 {
		compatible = "vendor,mt6577-sysirq";
		interrupt-controller;
		#interrupt-cells = <0x3>;
		interrupt-parent = <0xd>;
		reg = <0x0 0x10200a80 0x0 0x50>;
		linux,phandle = <0x1>;
		phandle = <0x1>;
	};
	
	md1_sim1_hot_plug_eint {
		linux,phandle = <0x4b>;
		phandle = <0x4b>;
		compatible = "vendor,md1_sim1_hot_plug_eint-eint";
		interrupts = <0x0 0x4>;
		debounce = <0x0 0x2710>;
		dedicated = <0x0 0x0>;
		src_pin = <0x0 0x1>;
		sockettype = <0x0 0x0>;
		status = "okay";
	};

	pinctrl@1000b000 {
		compatible = "vendor,xx6761-pinctrl";
		reg = <0x0 0x1000b000 0x0 0x1000>;
		vendor,pctl-regmap = <0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18>;
		pins-are-numbered;
		gpio-controller;
		#gpio-cells = <0x2>;
		interrupt-controller;
		#interrupt-cells = <0x4>;
		interrupts = <0x0 0x73 0x4>;
		linux,phandle = <0x37>;
		phandle = <0x37>;
	};
	
	timer {
		compatible = "arm,armv8-timer";
		interrupt-parent = <0xd>;
		interrupts = <0x1 0xd 0x8 0x1 0xe 0x8 0x1 0xb 0x8 0x1 0xa 0x8>;
		clock-frequency = <0xc65d40>;
	};

	mt-pmic {
		compatible = "vendor,mt_pmic";
		interrupt-controller;
		linux,phandle = <0x52>;
		phandle = <0x52>;
		interrupt-parent = <0x37>;
		interrupts = <0x90 0x4 0x90 0x1>;
		status = "okay";
		
		ldo_regulators {

			ldo_vcn28 {
				compatible = "regulator-fixed";
				regulator-name = "vcn28";
				regulator-min-microvolt = <0x2ab980>;
				regulator-max-microvolt = <0x2ab980>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x60>;
				phandle = <0x60>;
			};

			ldo_vcama {
				regulator-name = "vcama";
				regulator-min-microvolt = <0x2625a0>;
				regulator-max-microvolt = <0x2ab980>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x62>;
				phandle = <0x62>;
				regulator-default-on = <0x1>;
				status = "okay";
			};

			ldo_vcamd {
				regulator-name = "vcamd";
				regulator-min-microvolt = <0xf4240>;
				regulator-max-microvolt = <0x1b7740>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x63>;
				phandle = <0x63>;
				regulator-default-on = <0x1>;
				status = "okay";
			};

			ldo_vcamio {
				compatible = "regulator-fixed";
				regulator-name = "vcamio";
				regulator-min-microvolt = <0x1b7740>;
				regulator-max-microvolt = <0x1b7740>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x64>;
				phandle = <0x64>;
				regulator-default-on = <0x1>;
				status = "okay";
			};

			ldo_vldo28 {
				regulator-name = "vldo28";
				regulator-min-microvolt = <0x2ab980>;
				regulator-max-microvolt = <0x2dc6c0>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x65>;
				phandle = <0x65>;
				regulator-default-on = <0x1>;
				status = "okay";
			};

			ldo_vio28 {
				compatible = "regulator-fixed";
				regulator-name = "vio28";
				regulator-min-microvolt = <0x2ab980>;
				regulator-max-microvolt = <0x2ab980>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x6a>;
				phandle = <0x6a>;
			};

			ldo_vio18 {
				compatible = "regulator-fixed";
				regulator-name = "vio18";
				regulator-min-microvolt = <0x1b7740>;
				regulator-max-microvolt = <0x1b7740>;
				regulator-enable-ramp-delay = <0x108>;
				linux,phandle = <0x6b>;
				phandle = <0x6b>;
			};
		};
	};

	touch {
		compatible = "vendor,touch";
		linux,phandle = <0x72>;
		phandle = <0x72>;
		tpd-resolution = <0x2d0 0x500>;
		use-tpd-button = <0x0>;
		tpd-key-num = <0x3>;
		tpd-key-local = <0x8b 0xac 0x9e 0x0>;
		tpd-key-dim-local = <0x5a 0x373 0x64 0x28 0xe6 0x373 0x64 0x28 0x172 0x373 0x64 0x28 0x0 0x0 0x0 0x0>;
		tpd-max-touch-num = <0x5>;
		tpd-filter-enable = <0x1>;
		tpd-filter-pixel-density = <0x6a>;
		tpd-filter-custom-prameters = <0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0>;
		tpd-filter-custom-speed = <0x0 0x0 0x0>;
		pinctrl-names = "default", "state_eint_as_int", "state_eint_output0", "state_eint_output1", "state_rst_output0", "state_rst_output1";
		pinctrl-0 = <0xa5>;
		pinctrl-1 = <0xa6>;
		pinctrl-2 = <0xa7>;
		pinctrl-3 = <0xa8>;
		pinctrl-4 = <0xa9>;
		pinctrl-5 = <0xaa>;
		status = "okay";
		interrupt-parent = <0x37>;
		interrupts = <0x0 0x2 0x0 0x0>;
		vtouch-supply = <0x65>;
	};
	
	kd_camera_hw1@15040000 {
		compatible = "vendor,camera_hw";
		reg = <0x0 0x15040000 0x0 0x1000>;
		clocks = <0x19 0x58 0x19 0x59 0x19 0x5a 0x19 0x5b 0x19 0x16 0x19 0x15 0x19 0x1f 0x19 0x13 0x19 0x1e 0x19 0x14 0x1b 0x3c 0x3 0x3b 0xc 0x3b 0x10 0x3d 0x0 0x3e 0x0 0x3f 0x0 0x40 0x0 0x41 0x0 0x42 0x0 0x19 0x6f 0x19 0x1c 0x20 0x8>;
		clock-names = "CLK_TOP_CAMTG_SEL", "CLK_TOP_CAMTG1_SEL", "CLK_TOP_CAMTG2_SEL", "CLK_TOP_CAMTG3_SEL", "CLK_MCLK_6M", "CLK_MCLK_12M", "CLK_MCLK_13M", "CLK_MCLK_48M", "CLK_MCLK_52M", "CLK_MCLK_24M", "CLK_MCLK_26M", "CLK_CAM_SENINF_CG", "CLK_MIPI_C0_26M_CG", "CLK_MIPI_C1_26M_CG", "CLK_MIPI_ANA_0A_CG", "CLK_MIPI_ANA_0B_CG", "CLK_MIPI_ANA_1A_CG", "CLK_MIPI_ANA_1B_CG", "CLK_MIPI_ANA_2A_CG", "CLK_MIPI_ANA_2B_CG", "CLK_TOP_CAMTM_SEL_CG", "CLK_TOP_CAMTM_208_CG", "CLK_SCP_SYS_CAM";
		linux,phandle = <0x98>;
		phandle = <0x98>;
		pinctrl-names = "default", "cam0_rst0", "cam0_rst1", "cam0_pnd0", "cam0_pnd1", "cam1_rst0", "cam1_rst1", "cam1_pnd0", "cam1_pnd1", "cam2_rst0", "cam2_rst1", "cam2_pnd0", "cam2_pnd1", "cam0_vcamd_on", "cam0_vcamd_off", "cam1_vcamd_on", "cam1_vcamd_off", "cam1_vcama_on", "cam1_vcama_off", "cam2_vcama_on", "cam2_vcama_off", "cam0_mclk_on", "cam0_mclk_off", "cam1_mclk_on", "cam1_mclk_off", "cam2_mclk_on", "cam2_mclk_off";
		pinctrl-0 = <0xbd>;
		pinctrl-1 = <0xbe>;
		pinctrl-2 = <0xbf>;
		pinctrl-3 = <0xc0>;
		pinctrl-4 = <0xc1>;
		pinctrl-5 = <0xc2>;
		pinctrl-6 = <0xc3>;
		pinctrl-7 = <0xc4>;
		pinctrl-8 = <0xc5>;
		pinctrl-9 = <0xc6>;
		pinctrl-10 = <0xc7>;
		pinctrl-11 = <0xc8>;
		pinctrl-12 = <0xc9>;
		pinctrl-13 = <0xca>;
		pinctrl-14 = <0xcb>;
		pinctrl-15 = <0xcc>;
		pinctrl-16 = <0xcd>;
		pinctrl-17 = <0xce>;
		pinctrl-18 = <0xcf>;
		pinctrl-19 = <0xd0>;
		pinctrl-20 = <0xd1>;
		pinctrl-21 = <0xd2>;
		pinctrl-22 = <0xd3>;
		pinctrl-23 = <0xd4>;
		pinctrl-24 = <0xd5>;
		pinctrl-25 = <0xd6>;
		pinctrl-26 = <0xd7>;
		status = "okay";
		vcama-supply = <0x62>;
		vcama_sub-supply = <0x62>;
		vcamd-supply = <0x63>;
		vcamd_sub-supply = <0x63>;
		vcamio-supply = <0x64>;
		vcamio_sub-supply = <0x64>;
		vcamaf-supply = <0x65>;
	};	
	
	sspm@10440000 {
		compatible = "vendor,sspm";
		reg = <0x0 0x10440000 0x0 0x10000 0x0 0x10450000 0x0 0x100 0x0 0x10451000 0x0 0x8 0x0 0x10460000 0x0 0x100 0x0 0x10461000 0x0 0x8 0x0 0x10470000 0x0 0x100 0x0 0x10471000 0x0 0x8 0x0 0x10480000 0x0 0x100 0x0 0x10481000 0x0 0x8 0x0 0x10490000 0x0 0x100 0x0 0x10491000 0x0 0x8>;
		reg-names = "cfgreg", "mbox0_base", "mbox0_ctrl", "mbox1_base", "mbox1_ctrl", "mbox2_base", "mbox2_ctrl", "mbox3_base", "mbox3_ctrl", "mbox4_base", "mbox4_ctrl";
		interrupts = <0x0 0x92 0x4 0x0 0x95 0x4 0x0 0x96 0x4 0x0 0x97 0x4 0x0 0x98 0x4 0x0 0x99 0x4>;
		interrupt-names = "ipc", "mbox0", "mbox1", "mbox2", "mbox3", "mbox4";
	};
	
	msdc1_ins@0 {
		compatible = "vendor,xx6761-sdcard-ins";
		linux,phandle = <0x90>;
		phandle = <0x90>;
		interrupt-parent = <0x37>;
		interrupts = <0x1 0x2 0x1 0x0>;
		status = "okay";
	};

	dsi_te {
		compatible = "vendor, DSI_TE-eint";
		status = "okay";
		linux,phandle = <0x96>;
		phandle = <0x96>;
		interrupt-parent = <0x37>;
		interrupts = <0x2c 0x1 0x2c 0x1>;
	};
	
	mt6370_pmu_eint {
		linux,phandle = <0xa1>;
		phandle = <0xa1>;
		interrupt-parent = <0x37>;
		interrupts = <0xb 0x2 0xb 0x0>;
		status = "okay";
	};

	mt6370_pmu_dts {
		interrupt-controller;
		#interrupt-cells = <0x1>;
		mt6370,intr_gpio_num = <0xb>;
		mt6370,intr_gpio = <0x37 0xb 0x0>;
		linux,phandle = <0xa3>;
		phandle = <0xa3>;

		core {
			compatible = "vendor,mt6370_pmu_core";
			interrupt-names = "otp", "vdda_ovp", "vdda_uv";
			i2cstmr_rst_tmr = <0x0>;
			mrstb_en;
			mrstb_tmr = <0x3>;
			int_wdt = <0x0>;
			int_deg = <0x0>;
		};
	};
	
	__symbols__ {
		chosen = "/chosen";
		gic = "/interrupt-controller@0c000000";
		kd_camera_hw1 = "/kd_camera_hw1@15040000";
	};
};
```

二、关于dts中的interrupt

1、 interrupt-controller ：一个空的属性，定义该节点作为一个接收中断信号的设备。

2、 #interrupt-cells ：这是一个中断控制器节点的属性。它声明了该中断控制器的中断指示符中 cell 的个数。

它从 interrupt-parent 继承而来，如果此节点没有定义 interrupt-parent，则从它们的父节点中继承该属性。

3、 interrupt-parent ：这是一个设备节点的属性，包含一个指向该设备连接的中断控制器的 phandle。

那些没有 interrupt-parent 的节点则从它们的父节点中继承该属性。

4、 interrupts ：一个设备节点属性，包含一个中断指示符的列表，对应于该设备上的每个中断输出信号。

中断指示符是一个或多个 cell 的数据，由 #interrupt-cells 指定。

5、 #interrupt-cells 的个数

参考文档：

kernel-4.9/Documentation/devicetree/bindings/interrupt-controller/arm,gic-v3.txt

（1）#interrupt-cells = <0x3>;尽管cells为3，但是也可以只有2个value。

2个value时：
```c
interrupts = <0x0 0x4>;
第一个value的含义为：0 for SPI(Shared Peripheral Interrupt) interrupts, 1 for PPI(Private Peripheral Interrupt) interrupts
第二个value的含义为：中断触发类型
```

3个value时：
```c
interrupts = <0x1 0x7 0x8>;
interrupts = <0x0 0xbc 0x1 0x0 0xbd 0x1>;
interrupts = <0x1 0xd 0x8 0x1 0xe 0x8 0x1 0xb 0x8 0x1 0xa 0x8>;

第一个value的含义为：0 for SPI interrupts, 1 for PPI interrupts
第二个value的含义为：中断号
第三个value的含义为：中断触发类型
```

（2）#interrupt-cells = <0x4>;

4个value时：
```c
interrupts = <0x90 0x4 0x90 0x1>;
interrupts = <0xb 0x2 0xb 0x0>;
interrupts = <0xc 0x8 0xc 0x0>;
第一个value的含义为：中断号
第二个value的含义为：中断触发类型
第三个value的含义为：中断号
第四个value的含义为：0 for SPI interrupts, 1 for PPI interrupts
```

（3） 中断触发类型
```c
0x1 = low-to-high edge triggered，上升沿触发。
0x2 = high-to-low edge triggered，下降沿触发。
0x4 = active high level-sensitive，高电平触发。
0x8 = active low level-sensitive，低电平触发。
```

三、关于dts的reg

在dts中，1个cells表示一个32bit的数据，2个cells表示2个32bit的数据，即64bit。

```c
ap_dma@11000000 {
	#address-cells = <0x2>;
	#size-cells = <0x2>;
	compatible = "vendor,ap_dma";
	reg = <0x0 0x11000000 0x0 0x1000>;
	interrupts = <0x0 0x52 0x8>;
};
```

addr:0x0000000011000000

size: 0x0000000000001000

可以这样使用它：

```c
void __iomem *dma_base;
struct device_node *dma_node;
dma_node = of_find_compatible_node(NULL, NULL, "vendor,ap_dma");
dma_base = of_iomap(dma_node, 0);//它会调用 ioremap
```

四、关于 device_node 和 property

node 的定义为：

```c
/ {
    node1 {
        a-string-property = "A string";
        a-string-list-property = "first string", "second string";
        a-byte-data-property = [0x01 0x23 0x34 0x56];
        child-node1 {
            first-child-property;
            second-child-property = <1>;
            a-string-property = "Hello, world";
        };
        child-node2 {
        };
    };
    node2 {
        an-empty-property;
        a-cell-property = <1 2 3 4>; /* each number (cell) is a uint32 */
        child-node1 {
        };
    };
};
```

每个 device_node 中都有一个 phandle ，表示 label的引用，例如 pinctrl。下面是一个 device_node 。

```c
syscfg_pctl_1@10002000 {
	compatible = "vendor,xx6761-pctl-1-syscfg", "syscon";
	reg = <0x0 0x10002000 0x0 0x1000>;
	status = "okay";
	linux,phandle = <0x12>;
	phandle = <0x12>;
};
```

每个 device_node 有很多 property ：

```c
compatible = "vendor,xx6761-pctl-1-syscfg", "syscon";
compatible 是一个 struct property ，它的 name为 compatible ， value 为 "vendor,xx6761-pctl-1-syscfg" 和 "syscon" 。

status = "okay";
status 是一个 struct property ，它的 name为 status ， value 为 "okay"
```

2个结构体：

```c
struct device_node {
	const char *name;
	const char *type;
	phandle phandle;
	const char *full_name;
	struct fwnode_handle fwnode;

	struct	property *properties;
	struct	property *deadprops;/* removed properties */
	struct	device_node *parent;
	struct	device_node *child;
	struct	device_node *sibling;
	struct	kobject kobj;
	unsigned long _flags;
	void	*data;
};
```

```c
struct property {
	char	*name;
	int	length;
	void	*value;
	struct property *next;
	unsigned long _flags;
	unsigned int unique_id;
	struct bin_attribute attr;
};
```


