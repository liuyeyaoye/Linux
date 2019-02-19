


一、 machine ： 用于描述 regulator 在板级的硬件配置，在dts中定义参数。

```c
静态的描述 regulator 在板级的物理现状
struct regulator_init_data {

	//regulator的parent。用于级联regulator使用。
	const char *supply_regulator;
	
	//此regulator的约束，比如输出电压范围，输出电流范围等。
	struct regulation_constraints constraints;
	
	//此regulator提供的consumer的个数，也就是控制外设的个数。	
	int num_consumer_supplies;
	
	//使用此结构确定regulator和consumer之间的联系。	
	struct regulator_consumer_supply *consumer_supplies;
	
	//regulator注册时候的回调函数。	
	int (*regulator_init)(void *driver_data);
	
	//regulator_init回调函数的参数	
	void *driver_data;
};
```

```c
该 regulator 的物理限制
struct regulation_constraints {

	//描述该约束的名字
	const char *name;
	
	//最小和最大的输出电压
	int min_uV;
	int max_uV;
	
	//consumer看到的电源和实际电源之间的偏移值，用于电源补偿
	int uV_offset;
	
	//最小和最大的输出电流
	int min_uA;
	int max_uA;

/*
该regulator支持的操作模式：
#define REGULATOR_MODE_FAST 		0x1  //快速改变模式
#define REGULATOR_MODE_NORMAL   	0x2  //正常模式，大多数驱动都使用这种模式
#define REGULATOR_MODE_IDLE     		0x4  //设备在idle状态，regulator给设备提供服务
#define REGULATOR_MODE_STANDBY  	0x8  //设备在standby状态，regulator给设备提供服务
*/
	unsigned int valid_modes_mask;

/*
该regulator支持的操作：
#define REGULATOR_CHANGE_VOLTAGE 	0x1   //该regulator可以改变电压
#define REGULATOR_CHANGE_CURRENT 	0x2   //该regulator可以改变电流
#define REGULATOR_CHANGE_MODE 	0x4   //该regulator可以改变mode
#define REGULATOR_CHANGE_STATUS  	0x8   //该regulator可以改变状态，也就是enable/disable power
#define REGULATOR_CHANGE_DRMS    	0x10  //该regulator可以动态该变mode
#define REGULATOR_CHANGE_BYPASS  	0x20  //该regulator支持bypass mode
*/
	unsigned int valid_ops_mask;

	//表示该regulator的input是另一个regulator
	int input_uV;

	//代表该regulator的各种suspend状态。
	struct regulator_state state_disk;
	struct regulator_state state_mem;
	struct regulator_state state_standby;
	
	//suspend state to set at init
	suspend_state_t initial_state;

	//mode to set on startup
	unsigned int initial_mode;

	//改变电压到电源稳定后时间。因为硬件原因，改变电源后不能立刻就成功，其中需要有一定的延迟
	unsigned int ramp_delay;

	//是否在系统启动后一直使能
	unsigned always_on:1;
	
	//是否在boot阶段使能
	unsigned boot_on:1;
	
	/* apply uV constraint if min == max */
	unsigned apply_uV:1;
};
```

regulator 的物理限制一般在dts中描述，例如：

```c
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
```

二、 regulator driver

注册自己的 regulator 服务到 regulator core framework 中，给 consumer 提供服务。

regulator driver 通过函数

regulator_register(const struct regulator_desc *regulator_desc, const struct regulator_config *config)

注册 regulator_ops 到regulator core 。

第一个参数struct regulator_desc代表静态的regulator配置，所谓静态就是不再会改变的配置。

第二个参数代表regulator的动态配置信息。

```c
描述 regulator 的静态信息
struct regulator_desc {

	//该regulator的名字。
	const char *name;
	
	//该regulator parent的name，在级联时使用
	const char *supply_name;
	
	//用于标识该regulator，也可以标识系统有多少个regulator	
	int id;
	
	//表示regulator是否可以在电压约束范围内连续输出电压
	bool continuous_voltage_range;
	
	//通过ops.list_voltage函数获取可用的电压数量	
	unsigned n_voltages;
	
	//regulator的操作函数集合	
	struct regulator_ops *ops;
	
	//该regulator的中断号	
	int irq;
	
	//代表当前regulator的类型：电压或电流
	enum regulator_type type;
	
	struct module *owner;

	//如果是线性mapp的话，使用最低的selector获取的电压	
	unsigned int min_uV;
	
	//每个selector下电压增加step	
	unsigned int uV_step;
	
	//线性mapp下最小的selector。	
	unsigned int linear_min_sel;
	
	//电压改变之后需要多久时间稳定下来	
	unsigned int ramp_delay;
	
	//电压的mapp表
	const unsigned int *volt_table;
	
	unsigned int vsel_reg;
	unsigned int vsel_mask;
	unsigned int apply_reg;
	unsigned int apply_bit;
	unsigned int enable_reg;
	unsigned int enable_mask;
	bool enable_is_inverted;
	unsigned int bypass_reg;
	unsigned int bypass_mask;
	
	// regulator初始化所需要的时间
	unsigned int enable_time;
};
```

```c
regulator操作函数：
struct regulator_ops {
	int (*list_voltage) (struct regulator_dev *, unsigned selector);
	int (*set_voltage) (struct regulator_dev *, int min_uV, int max_uV, unsigned *selector);
	int (*map_voltage)(struct regulator_dev *, int min_uV, int max_uV);
	int (*set_voltage_sel) (struct regulator_dev *, unsigned selector);
	int (*get_voltage) (struct regulator_dev *);
	int (*get_voltage_sel) (struct regulator_dev *);
	int (*set_current_limit) (struct regulator_dev *, int min_uA, int max_uA);
	int (*get_current_limit) (struct regulator_dev *);
	int (*enable) (struct regulator_dev *);
	int (*disable) (struct regulator_dev *);
	int (*is_enabled) (struct regulator_dev *);
	int (*set_mode) (struct regulator_dev *, unsigned int mode);
	unsigned int (*get_mode) (struct regulator_dev *);
	int (*enable_time) (struct regulator_dev *);
	int (*set_ramp_delay) (struct regulator_dev *, int ramp_delay);
	int (*set_voltage_time_sel) (struct regulator_dev *, unsigned int old_selector, unsigned int new_selector);
	int (*get_status)(struct regulator_dev *);
	unsigned int (*get_optimum_mode) (struct regulator_dev *, int input_uV,  int output_uV, int load_uA);
	int (*set_bypass)(struct regulator_dev *dev, bool enable);
	int (*get_bypass)(struct regulator_dev *dev, bool *enable);
	int (*set_suspend_voltage) (struct regulator_dev *, int uV);
	int (*set_suspend_enable) (struct regulator_dev *);
	int (*set_suspend_disable) (struct regulator_dev *);
	int (*set_suspend_mode) (struct regulator_dev *, unsigned int mode);
};
```

```c
描述 regulator 的动态信息
struct regulator_config {
	struct device *dev;
	
	//板级的相关初始化信息，通过解析DT，保存在此
	const struct regulator_init_data *init_data;
	void *driver_data;
	struct device_node *of_node;
	struct regmap *regmap;

	int ena_gpio;
	unsigned int ena_gpio_invert:1;
	unsigned int ena_gpio_flags;
};
```

```c
从设备的角度描述 regulator , regulator_register 的返回值类型。
struct regulator_dev {
	const struct regulator_desc *desc;
	int exclusive;
	u32 use_count;
	u32 open_count;
	u32 bypass_count;

	/* lists we belong to */
	struct list_head list; /* list of all regulators */

	/* lists we own */
	struct list_head consumer_list; /* consumers we supply */

	struct blocking_notifier_head notifier;
	struct mutex mutex; /* consumer lock */
	struct module *owner;
	struct device dev;
	struct regulation_constraints *constraints;
	struct regulator *supply;	/* for tree */
	struct regmap *regmap;

	struct delayed_work disable_work;
	int deferred_disables;

	void *reg_data;		/* regulator_dev data */

	struct dentry *debugfs;

	struct regulator_enable_gpio *ena_pin;
	unsigned int ena_gpio_state:1;
};
```

三、 consumer

```c
从 regulator consumer 的角度，抽象 regulator 设备
/*
 * One for each consumer device.
 */
struct regulator {
	struct device *dev;
	struct list_head list;
	unsigned int always_on:1;
	unsigned int bypass:1;
	int uA_load;
	int min_uV;
	int max_uV;
	char *supply_name;
	struct device_attribute dev_attr;
	struct regulator_dev *rdev;
	struct dentry *debugfs;
};
```

四、 core

提供 regulator 接口，如 regulator_register()， regulator_get()， regulator_put()， regulator_enable()，regulator_disable()， regulator_set_voltage()， regulator_is_enabled()。

五、 vendor pmic 实现

```c
struct vendor_regulator {
	struct regulator_desc desc;
	struct regulator_init_data init_data;
	struct regulator_config config;
	struct device_attribute en_att;
	struct device_attribute voltage_att;
	struct regulator_dev *rdev;
	PMU_FLAGS_LIST_ENUM en_reg;
	PMU_FLAGS_LIST_ENUM vol_reg;
	PMU_FLAGS_LIST_ENUM qi_en_reg;
	PMU_FLAGS_LIST_ENUM qi_vol_reg;	
	const void *pvoltages;
	bool isUsedable;
	struct regulator *reg;
};
```

参考资料：

https://blog.csdn.net/longwang155069/article/details/53129378


