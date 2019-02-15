
```c
mtk input子系统

〇、相关的知识

0、 list_add_tail(struct list_head *new, struct list_head *head);
		将 new 添加到 head 的尾部。

1、 list_for_each_entry(pos, head, member)  ：循环遍历每一个 pos 的子项 member 。原型如下：

#define list_for_each_entry(pos, head, member)              \  
    for (pos = list_entry((head)->next, typeof(*pos), member);   \  
         prefetch(pos->member.next), &pos->member != (head);  \  
         pos = list_entry(pos->member.next, typeof(*pos), member))  

prefetch的含义是告诉cpu那些元素有可能马上就要用到，告诉cpu预取一下，这样可以提高速度，用于预取以提高遍历速度；

#define list_entry(ptr, type, member) 	    container_of(ptr, type, member)       
//type 为某个结构体的类型， member 为结构体的某个成员， ptr 为 member 的指针。根据 ptr 获取整个结构体的地址。

2、Linux input子系统从上到下分为3层：事件处理层(event_handler)， input核心层，input驱动层。
1）input 驱动层与硬件交互，获取 touch panel 的数据，中断操作等。
2）input 核心层提供接口和操作函数给事件处理层和 input 驱动层，将 input 驱动层的数据传给事件处理层。
3）事件处理层与用户空间的应用程序交互，应用程序只需关心 event_handler 的文件和设备节点接口，无需关心底层具体操作。

3、 attention：
1） input_register_handler() 是基于 module_init() ，在 input_register_handler() 函数中，有：
    list_add_tail(&handler->node, &input_handler_list); 
    list_for_each_entry(dev, &input_dev_list, node)  		
        input_attach_handler(dev, handler);	
		
1.1）这里是 handler 主动与 input_dev 进行连接，由于此时 input_dev_list 上面还没有 input_dev 挂载，所以handler 连接 input_dev 会失败。

2)	input_register_device() 是基于 late_initcall() ，在 input_register_device() 函数中，有：
    list_add_tail(&dev->node, &input_dev_list);		
    list_for_each_entry(handler, &input_handler_list, node)				
        input_attach_handler(dev, handler);

2.1） 这里是 input_dev 主动与 handler 进行连接，由于此时 input_handler_list 上面挂载了一个 handler ，所以在这里， handler 和 input_dev 连接成功，调用 evdev_connect() 函数。

3） 在 evdev_connect() 函数中，会调用 input_register_handle() 函数注册一个 handle ， input_handle 是连接 input_dev 和 input_handler 的桥梁。只有当 input_dev 和 input_handler 连接成功时，才会注册一个 input_handle 结构体，将二者关连。

4）有4个重要的结构体：
input_dev 、 input_handler 、 input_handle 、 evdev 
4.1) input_dev  ： input 驱动层的结构体
4.2) input_handler ：
4.3) input_handle 	：
4.4) evdev  ： 包含一个字符设备，提供给应用程序的接口。


一、三个重要的结构体
0、 evdev 结构体：
struct evdev {
	int open;
	struct input_handle handle;
	wait_queue_head_t wait;
	struct evdev_client __rcu *grab;
	struct list_head client_list;
	spinlock_t client_lock; /* protects client_list */
	struct mutex mutex;
	struct device dev;
	struct cdev cdev;
	bool exist;
};

1、 input_handle 结构体：
struct input_handle {
	void *private;
	int open;
	const char *name;
	struct input_dev *dev;
	struct input_handler *handler;
	struct list_head	d_node;
	struct list_head	h_node;
};

2、 input_handler 结构体：
struct input_handler {
	void *private;
	void (*event)(struct input_handle *handle, unsigned int type, unsigned int code, int value);
	void (*events)(struct input_handle *handle,   const struct input_value *vals, unsigned int count);
	bool (*filter)(struct input_handle *handle, unsigned int type, unsigned int code, int value);
	bool (*match)(struct input_handler *handler, struct input_dev *dev);
	int (*connect)(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id);
	void (*disconnect)(struct input_handle *handle);
	void (*start)(struct input_handle *handle);
	bool legacy_minors;
	int minor;
	const char *name;
	const struct input_device_id *id_table;
	struct list_head	h_list;
	struct list_head	node;
};

3、 input_dev 结构体：
struct input_dev {
	const char *name;
	const char *phys;
	const char *uniq;
	struct input_id id;
	unsigned long propbit[BITS_TO_LONGS(INPUT_PROP_CNT)];
	unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
	unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
	unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
	unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];
	unsigned long mscbit[BITS_TO_LONGS(MSC_CNT)];
	unsigned long ledbit[BITS_TO_LONGS(LED_CNT)];
	unsigned long sndbit[BITS_TO_LONGS(SND_CNT)];
	unsigned long ffbit[BITS_TO_LONGS(FF_CNT)];
	unsigned long swbit[BITS_TO_LONGS(SW_CNT)];
	unsigned int hint_events_per_packet;
	unsigned int keycodemax;
	unsigned int keycodesize;
	void *keycode;
	int (*setkeycode)(struct input_dev *dev,  const struct input_keymap_entry *ke,  unsigned int *old_keycode);
	int (*getkeycode)(struct input_dev *dev,  struct input_keymap_entry *ke);
	struct ff_device *ff;
	unsigned int repeat_key;
	struct timer_list timer;
	int rep[REP_CNT];
	struct input_mt *mt;
	struct input_absinfo *absinfo;
	unsigned long key[BITS_TO_LONGS(KEY_CNT)];
	unsigned long led[BITS_TO_LONGS(LED_CNT)];
	unsigned long snd[BITS_TO_LONGS(SND_CNT)];
	unsigned long sw[BITS_TO_LONGS(SW_CNT)];
	int (*open)(struct input_dev *dev);
	void (*close)(struct input_dev *dev);
	int (*flush)(struct input_dev *dev, struct file *file);
	int (*event)(struct input_dev *dev, unsigned int type, unsigned int code, int value);
	struct input_handle __rcu *grab;
	spinlock_t event_lock;
	struct mutex mutex;
	unsigned int users;
	bool going_away;
	struct device dev;
	struct list_head	h_list;
	struct list_head	node;
	unsigned int num_vals;
	unsigned int max_vals;
	struct input_value *vals;
	bool devres_managed;
};

一、input子系统架构(分层架构)：

--------------------------------------------------------------------------------------------------------------------------
APP层：应用程序文件接口
/dev/input/event6
…………
/dev/input/event0
/dev/input/ts0
/dev/input/mouse0
/dev/ttyn
-------------------------------------------------------------------------------------------------------------------------
事件处理层(event_handler)：
evdev.c
tsdev.c
mousedev.c
keyboard.c
event_handler 实现了多个事件的处理驱动：如 处理通用设备的 evdev ，处理鼠标类输入设备的 mousedev ，处理游戏杆 joydev 。
--------------------------------------------------------------------------------------------------------------------------
input核心层(input core)：
input.c(包括 input_event() 函数)
--------------------------------------------------------------------------------------------------------------------------
input driver层：
1) mtk tp driver 
2) 3rd tp driver
--------------------------------------------------------------------------------------------------------------------------
硬件层：		
cap touch panel
--------------------------------------------------------------------------------------------------------------------------

二、事件处理层 event_handler 的程序 evdev.c 
0、相关的结构体
0.1 文件操作接口， evdev 提供给应用程序的接口
static const struct file_operations evdev_fops = {
	.owner		= THIS_MODULE,
	.read		= evdev_read,
	.write		= evdev_write,
	.poll		= evdev_poll,
	.open		= evdev_open,
	.release	= evdev_release,
	.unlocked_ioctl	= evdev_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= evdev_ioctl_compat,
#endif
	.fasync		= evdev_fasync,
	.flush		= evdev_flush,
	.llseek		= no_llseek,
};

struct evdev {
	int open;
	struct input_handle handle;
	wait_queue_head_t wait;
	struct evdev_client __rcu *grab;
	struct list_head client_list;
	spinlock_t client_lock; /* protects client_list */
	struct mutex mutex;
	struct device dev;
	struct cdev cdev;
	bool exist;
};

struct input_handle {
	void *private;
	int open;
	const char *name;
	struct input_dev *dev;
	struct input_handler *handler;
	struct list_head	d_node;
	struct list_head	h_node;
};

static const struct input_device_id evdev_ids[] = {
	{ .driver_info = 1 },	/* Matches all devices */
	{ },			/* Terminating zero entry */
};

static struct input_handler evdev_handler = {
	.event		= evdev_event,
	.events		= evdev_events,
	.connect	= evdev_connect,
	.disconnect	= evdev_disconnect,
	.legacy_minors	= true,
	.minor		= EVDEV_MINOR_BASE,		//64
	.name		= "evdev",			//name is to be shown in /proc/bus/input/handlers
	.id_table	= evdev_ids,
};

1、 module_init(evdev_init);
static int __init evdev_init(void)
{
	return input_register_handler(&evdev_handler);			
    //一个 handler 可以处理多个 devices
}

2、  evdev_connect 函数(在 input_register_device 中被调用)
static int evdev_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id)
{
	struct evdev *evdev;
	int minor;
	int dev_no;
	int error;

	minor = input_get_new_minor(EVDEV_MINOR_BASE, EVDEV_MINORS, true);
	if (minor < 0) {
		error = minor;
		pr_err("failed to reserve new minor: %d\n", error);
		return error;
	}

	evdev = kzalloc(sizeof(struct evdev), GFP_KERNEL);
	if (!evdev) {
		error = -ENOMEM;
		goto err_free_minor;
	}

	INIT_LIST_HEAD(&evdev->client_list);
	spin_lock_init(&evdev->client_lock);
	mutex_init(&evdev->mutex);
	init_waitqueue_head(&evdev->wait);
	evdev->exist = true;

	dev_no = minor;
	/* Normalize device number if it falls into legacy range */
	if (dev_no < EVDEV_MINOR_BASE + EVDEV_MINORS)
		dev_no -= EVDEV_MINOR_BASE;
	dev_set_name(&evdev->dev, "event%d", dev_no);

	evdev->handle.dev = input_get_device(dev);
	evdev->handle.name = dev_name(&evdev->dev);
	evdev->handle.handler = handler;
	evdev->handle.private = evdev;

	evdev->dev.devt = MKDEV(INPUT_MAJOR, minor);
	evdev->dev.class = &input_class;
	evdev->dev.parent = &dev->dev;
	evdev->dev.release = evdev_free;
	device_initialize(&evdev->dev);

	error = input_register_handle(&evdev->handle);
	if (error)
		goto err_free_evdev;

	cdev_init(&evdev->cdev, &evdev_fops);
	evdev->cdev.kobj.parent = &evdev->dev.kobj;
	error = cdev_add(&evdev->cdev, evdev->dev.devt, 1);
	if (error)
		goto err_unregister_handle;

	error = device_add(&evdev->dev);
	if (error)
		goto err_cleanup_evdev;

	return 0;

 err_cleanup_evdev:
	evdev_cleanup(evdev);
 err_unregister_handle:
	input_unregister_handle(&evdev->handle);
 err_free_evdev:
	put_device(&evdev->dev);
 err_free_minor:
	input_free_minor(minor);
	return error;
}

int input_register_handle(struct input_handle *handle)
{
	struct input_handler *handler = handle->handler;
	struct input_dev *dev = handle->dev;
	int error;

	/*
	 * We take dev->mutex here to prevent race with
	 * input_release_device().
	 */
	error = mutex_lock_interruptible(&dev->mutex);
	if (error)
		return error;

	/*
	 * Filters go to the head of the list, normal handlers
	 * to the tail.
	 */
	if (handler->filter)
		list_add_rcu(&handle->d_node, &dev->h_list);
	else
		list_add_tail_rcu(&handle->d_node, &dev->h_list);  	
        //将 handle 添加到 input_dev 的链表。

	mutex_unlock(&dev->mutex);

	/*
	 * Since we are supposed to be called from ->connect()
	 * which is mutually exclusive with ->disconnect()
	 * we can't be racing with input_unregister_handle()
	 * and so separate lock is not needed here.
	 */
	list_add_tail_rcu(&handle->h_node, &handler->h_list);				
    //将 handle 添加到 input_handler 的链表。

	if (handler->start)	//handler->start = NULL 
		handler->start(handle);
	return 0;
}

三、input核心层的程序 input.c 和上报函数 input_event() 函数
0、相关的结构体
struct class input_class = {
	.name		= "input",
	.devnode	= input_devnode,
};

1、subsys_initcall(input_init);
static int __init input_init(void)
{
	int err;

	err = class_register(&input_class);			//创建  /sys/class/input 目录结构
	if (err) {
		pr_err("unable to register input_dev class\n");
		return err;
	}

	err = input_proc_init();
	if (err)
		goto fail1;

	err = register_chrdev_region(MKDEV(INPUT_MAJOR, 0),  INPUT_MAX_CHAR_DEVICES, "input");			//INPUT_MAJOR=13    ,   INPUT_MAX_CHAR_DEVICES=1024
//register a range of device numbers
	if (err) {
		pr_err("unable to register char major %d", INPUT_MAJOR);
		goto fail2;
	}

	return 0;

 fail2:	input_proc_exit();
 fail1:	class_unregister(&input_class);
	return err;
}

static int __init input_proc_init(void)
{
	struct proc_dir_entry *entry;

	proc_bus_input_dir = proc_mkdir("bus/input", NULL);			//创建 /proc/bus/input 目录结构
	if (!proc_bus_input_dir)
		return -ENOMEM;

	entry = proc_create("devices", 0, proc_bus_input_dir,   &input_devices_fileops);		//创建 proc文件： /proc/bus/input/devices
	if (!entry)
		goto fail1;

	entry = proc_create("handlers", 0, proc_bus_input_dir,  &input_handlers_fileops);      //创建 proc 文件： /proc/bus/input/handlers
	if (!entry)
		goto fail2;

	return 0;

 fail2:	remove_proc_entry("devices", proc_bus_input_dir);
 fail1: remove_proc_entry("bus/input", NULL);
	return -ENOMEM;
}

2、  input_register_handler()/input_allocate_device()/input_register_device()/input_event() 函数体

1）相关的结构体

static struct input_handler evdev_handler = {
	.event		= evdev_event,
	.events		= evdev_events,
	.connect	= evdev_connect,
	.disconnect	= evdev_disconnect,
	.legacy_minors	= true,
	.minor		= EVDEV_MINOR_BASE,		//64
	.name		= "evdev",			//name is to be shown in /proc/bus/input/handlers
	.id_table	= evdev_ids,
};

2） input_register_handler() 函数体：
int input_register_handler(struct input_handler *handler)
{
	struct input_dev *dev;
	int error;

	error = mutex_lock_interruptible(&input_mutex);		//获取一个可以被中断的信号量
	if (error)
		return error;

	INIT_LIST_HEAD(&handler->h_list);		//初始化 handler 双链表 ， 这个双链表 handler->h_list 用于 handler 和其他的 handler 互相联系用。

	list_add_tail(&handler->node, &input_handler_list);  //将 handler->node 插入到 input_handler_list 的尾部。

//list_for_each_entry(pos, head, member)  ：循环遍历每一个 pos 的子项 member 。	
	list_for_each_entry(dev, &input_dev_list, node)  		//循环遍历 input_dev_list 上的每一个 dev ， node 为 dev 的一个成员。
		input_attach_handler(dev, handler);							
//这里是 handler 主动与 input_dev_list 上的所有 input_dev 进行遍历连接，然后调用 handler->connect ，即 evdev_connect() 函数。
//但是此时 input_dev_list 上面还没有 input_dev 挂载，所以这段代码没有意义。

	input_wakeup_procfs_readers();

	mutex_unlock(&input_mutex);
	return 0;
}

static inline void input_wakeup_procfs_readers(void)
{
	input_devices_state++;
	wake_up(&input_devices_poll_wait);
}

//将 input_dev 与 input_handler 进行连接
static int input_attach_handler(struct input_dev *dev, struct input_handler *handler)
{
	const struct input_device_id *id;
	int error;

	id = input_match_device(handler, dev);
	if (!id)
		return -ENODEV;  //直接退出。

	error = handler->connect(handler, dev, id);		//调用 evdev_connect() 函数
	if (error && error != -ENODEV)
		pr_err("failed to attach handler %s to device %s, error: %d\n",  handler->name, kobject_name(&dev->dev.kobj), error);

	return error;
}


static const struct input_device_id *input_match_device(struct input_handler *handler,	struct input_dev *dev)
{
	const struct input_device_id *id;

	for (id = handler->id_table; id->flags || id->driver_info; id++) {			//id->driver_info=1

		if (id->flags & INPUT_DEVICE_ID_MATCH_BUS)
			if (id->bustype != dev->id.bustype)
				continue;

		if (id->flags & INPUT_DEVICE_ID_MATCH_VENDOR)
			if (id->vendor != dev->id.vendor)
				continue;

		if (id->flags & INPUT_DEVICE_ID_MATCH_PRODUCT)
			if (id->product != dev->id.product)
				continue;

		if (id->flags & INPUT_DEVICE_ID_MATCH_VERSION)
			if (id->version != dev->id.version)
				continue;

		if (!bitmap_subset(id->evbit, dev->evbit, EV_MAX))		//如果 dev->evbit 是 id->evbit 的子集并且  EV_MAX < BITS_PER_LONG  ，返回1；否则返回0
			continue;

		if (!bitmap_subset(id->keybit, dev->keybit, KEY_MAX))
			continue;

		if (!bitmap_subset(id->relbit, dev->relbit, REL_MAX))
			continue;

		if (!bitmap_subset(id->absbit, dev->absbit, ABS_MAX))
			continue;

		if (!bitmap_subset(id->mscbit, dev->mscbit, MSC_MAX))
			continue;

		if (!bitmap_subset(id->ledbit, dev->ledbit, LED_MAX))
			continue;

		if (!bitmap_subset(id->sndbit, dev->sndbit, SND_MAX))
			continue;

		if (!bitmap_subset(id->ffbit, dev->ffbit, FF_MAX))
			continue;

		if (!bitmap_subset(id->swbit, dev->swbit, SW_MAX))
			continue;

		if (!handler->match || handler->match(handler, dev))		//handler->match =0 ，条件为真， return id 。
			return id;
	}

	return NULL;
}


3）  input_allocate_device() 函数体
相关的结构体：

static struct device_type input_dev_type = {
	.groups		= input_dev_attr_groups,
	.release	= input_dev_release,
	.uevent		= input_dev_uevent,
#ifdef CONFIG_PM_SLEEP
	.pm		= &input_dev_pm_ops,
#endif
};

struct class input_class = {
	.name		= "input",
	.devnode	= input_devnode,
};

struct input_dev *input_allocate_device(void)
{
	static atomic_t input_no = ATOMIC_INIT(0);
	struct input_dev *dev;

	dev = kzalloc(sizeof(struct input_dev), GFP_KERNEL);
	if (dev) {
		dev->dev.type = &input_dev_type;
		dev->dev.class = &input_class;
		device_initialize(&dev->dev);
		mutex_init(&dev->mutex);
		spin_lock_init(&dev->event_lock);
		init_timer(&dev->timer);
		INIT_LIST_HEAD(&dev->h_list);
		INIT_LIST_HEAD(&dev->node);

		dev_set_name(&dev->dev, "input%lu",  (unsigned long) atomic_inc_return(&input_no) - 1);

		__module_get(THIS_MODULE);
	}

	return dev;
}



4） input_register_device() 函数体 

int input_register_device(struct input_dev *dev)
{
	struct input_devres *devres = NULL;
	struct input_handler *handler;
	unsigned int packet_size;
	const char *path;
	int error;

	if (dev->devres_managed) {
		devres = devres_alloc(devm_input_device_unregister,   sizeof(struct input_devres), GFP_KERNEL);
		if (!devres)
			return -ENOMEM;

		devres->input = dev;
	}

	/* Every input device generates EV_SYN/SYN_REPORT events. */
	__set_bit(EV_SYN, dev->evbit);

	/* KEY_RESERVED is not supposed to be transmitted to userspace. */
	__clear_bit(KEY_RESERVED, dev->keybit);

	/* Make sure that bitmasks not mentioned in dev->evbit are clean. */
	input_cleanse_bitmasks(dev);

	packet_size = input_estimate_events_per_packet(dev);
	if (dev->hint_events_per_packet < packet_size)
		dev->hint_events_per_packet = packet_size;

	dev->max_vals = dev->hint_events_per_packet + 2;
	dev->vals = kcalloc(dev->max_vals, sizeof(*dev->vals), GFP_KERNEL);
	if (!dev->vals) {
		error = -ENOMEM;
		goto err_devres_free;
	}

	/*
	 * If delay and period are pre-set by the driver, then autorepeating
	 * is handled by the driver itself and we don't do it in input.c.
	 */
	if (!dev->rep[REP_DELAY] && !dev->rep[REP_PERIOD]) {
		dev->timer.data = (long) dev;
		dev->timer.function = input_repeat_key;
		dev->rep[REP_DELAY] = 250;
		dev->rep[REP_PERIOD] = 33;
	}

	if (!dev->getkeycode)
		dev->getkeycode = input_default_getkeycode;

	if (!dev->setkeycode)
		dev->setkeycode = input_default_setkeycode;

	error = device_add(&dev->dev);
	if (error)
		goto err_free_vals;

	path = kobject_get_path(&dev->dev.kobj, GFP_KERNEL);
	pr_info("%s as %s\n",
		dev->name ? dev->name : "Unspecified device",
		path ? path : "N/A");
	kfree(path);

	error = mutex_lock_interruptible(&input_mutex);
	if (error)
		goto err_device_del;

	list_add_tail(&dev->node, &input_dev_list);		//将这个 input_dev 添加到 input_dev_list 链表

	list_for_each_entry(handler, &input_handler_list, node)				
		input_attach_handler(dev, handler);
//循环遍历 input_handler_list  上的每一个 handler ，调用 input_attach_handler()  与 input_dev  attach ，此时 input_handler_list 不为空。

	input_wakeup_procfs_readers();

	mutex_unlock(&input_mutex);

	if (dev->devres_managed) {
		dev_dbg(dev->dev.parent, "%s: registering %s with devres.\n",	__func__, dev_name(&dev->dev));
		devres_add(dev->dev.parent, devres);
	}
	return 0;

err_device_del:
	device_del(&dev->dev);
err_free_vals:
	kfree(dev->vals);
	dev->vals = NULL;
err_devres_free:
	devres_free(devres);
	return error;
}




5） input_event() 函数体

void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
{
	unsigned long flags;

	if (is_event_supported(type, dev->evbit, EV_MAX)) {

		spin_lock_irqsave(&dev->event_lock, flags);
		input_handle_event(dev, type, code, value);
		spin_unlock_irqrestore(&dev->event_lock, flags);
	}
}

static void input_handle_event(struct input_dev *dev,   unsigned int type, unsigned int code, int value)
{
	int disposition;

	disposition = input_get_disposition(dev, type, code, &value);

	if ((disposition & INPUT_PASS_TO_DEVICE) && dev->event)
		dev->event(dev, type, code, value);

	if (!dev->vals)
		return;

	if (disposition & INPUT_PASS_TO_HANDLERS) {
		struct input_value *v;

		if (disposition & INPUT_SLOT) {
			v = &dev->vals[dev->num_vals++];
			v->type = EV_ABS;
			v->code = ABS_MT_SLOT;
			v->value = dev->mt->slot;
		}

		v = &dev->vals[dev->num_vals++];
		v->type = type;
		v->code = code;
		v->value = value;
	}

	if (disposition & INPUT_FLUSH) {
		if (dev->num_vals >= 2)
			input_pass_values(dev, dev->vals, dev->num_vals);
		dev->num_vals = 0;
	} else if (dev->num_vals >= dev->max_vals - 2) {
		dev->vals[dev->num_vals++] = input_value_sync;
		input_pass_values(dev, dev->vals, dev->num_vals);
		dev->num_vals = 0;
	}

}







6）  input_report_key()/input_report_rel()/input_report_abs()
static inline void input_report_key(struct input_dev *dev, unsigned int code, int value)
{
	input_event(dev, EV_KEY, code, !!value);
}

static inline void input_report_rel(struct input_dev *dev, unsigned int code, int value)
{
	input_event(dev, EV_REL, code, value);
}

static inline void input_report_abs(struct input_dev *dev, unsigned int code, int value)
{
	input_event(dev, EV_ABS, code, value);
}



四、input 驱动层
0、相关的结构体

struct tpd_device *tpd = 0;

struct tpd_device {
	struct device *tpd_dev;
	struct regulator *reg;
	struct regulator *io_reg;
	struct input_dev *dev;
	struct input_dev *kpd;
	struct timer_list timer;
	struct tasklet_struct tasklet;
	int btn_state;
};

1、 late_initcall(mtk_tp_probe)
static int tpd_probe(struct platform_device *pdev)
{
	tpd = kmalloc(sizeof(struct tpd_device), GFP_KERNEL);
	memset(tpd, 0, sizeof(struct tpd_device));
	
	tpd->dev = input_allocate_device();
	tpd->dev->name = TPD_DEVICE;
	set_bit(EV_ABS, tpd->dev->evbit);
	set_bit(EV_KEY, tpd->dev->evbit);
	set_bit(ABS_X, tpd->dev->absbit);
	set_bit(ABS_Y, tpd->dev->absbit);
	set_bit(ABS_PRESSURE, tpd->dev->absbit);
	
	input_register_device(tpd->dev);
}


2、 3rd tp driver interrupt handle 

static int fts_input_report_b(struct fts_ts_data *data)
{
    int i = 0;
    int uppoint = 0;
    int touchs = 0;
    bool va_reported = false;
    u32 max_touch_num = tpd_dts_data.touch_max_num;
    u32 key_y_coor = TPD_RES_Y;
    struct ts_event *events = data->events;

    for (i = 0; i < data->touch_point; i++) {
        if (KEY_EN && TOUCH_IS_KEY(events[i].y, key_y_coor)) {
            fts_input_report_key(data, i);
            continue;
        }

        if (events[i].id >= max_touch_num)
            break;

        va_reported = true;
        input_mt_slot(tpd->dev, events[i].id);

        if (EVENT_DOWN(events[i].flag)) {
            input_mt_report_slot_state(tpd->dev, MT_TOOL_FINGER, true);

#if FTS_REPORT_PRESSURE_EN
            if (events[i].p <= 0) {
                events[i].p = 0x3f;
            }
            input_report_abs(tpd->dev, ABS_MT_PRESSURE, events[i].p);
#endif
            if (events[i].area <= 0) {
                events[i].area = 0x09;
            }
            input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, events[i].area);
            input_report_abs(tpd->dev, ABS_MT_POSITION_X, events[i].x);
            input_report_abs(tpd->dev, ABS_MT_POSITION_Y, events[i].y);

            touchs |= BIT(events[i].id);
            data->touchs |= BIT(events[i].id);

            FTS_DEBUG("[B]P%d(%d, %d)[p:%d,tm:%d] DOWN!", events[i].id, events[i].x,
                      events[i].y, events[i].p, events[i].area);
        } 
		
		else {
            uppoint++;
            input_mt_report_slot_state(tpd->dev, MT_TOOL_FINGER, false);
            data->touchs &= ~BIT(events[i].id);
            FTS_DEBUG("[B]P%d UP!", events[i].id);
        }
    }

    if (unlikely(data->touchs ^ touchs)) {
        for (i = 0; i < max_touch_num; i++)  {
            if (BIT(i) & (data->touchs ^ touchs)) {
                FTS_DEBUG("[B]P%d UP!", i);
                va_reported = true;
                input_mt_slot(tpd->dev, i);
                input_mt_report_slot_state(tpd->dev, MT_TOOL_FINGER, false);
            }
        }
    }
    data->touchs = touchs;

    if (va_reported) {
        /* touchs==0, there's no point but key */
        if (EVENT_NO_DOWN(data) || (!touchs)) {
            FTS_DEBUG("[B]Points All Up!");
            input_report_key(tpd->dev, BTN_TOUCH, 0);
        } 
		else {
            input_report_key(tpd->dev, BTN_TOUCH, 1);
        }
        input_sync(tpd->dev);
    }

    return 0;
}



五、 总结
1. input 子系统一共有3个驱动模块：

1） input 核心层
subsys_initcall(input_init);
这里只注册一个 input 类、注册一组 input 字符设备编号、初始化 input proc 文件系统。

2） 事件处理层 event_handler
module_init(evdev_init);
这里只注册一个 input_handler: input_register_handler(&evdev_handler);
一个 evdev_handler 可以处理多个 input_dev.

int input_register_handler(struct input_handler *handler)
{
	......
	list_add_tail(&handler->node, &input_handler_list);  
	//将 handler->node 插入到 input_handler_list 的尾部。

	//遍历 input_dev_list 上的每一个 dev ， node 为 dev 的一个成员。
	list_for_each_entry(dev, &input_dev_list, node)  		
		input_attach_handler(dev, handler);							
	//这里是 handler 主动与 input_dev_list 上的所有 input_dev 进行遍历连接，然后调用 handler->connect ，即 evdev_connect() 函数。
	//但是此时 input_dev_list 上面还没有 input_dev 挂载，所以这段代码没有效果。
	......	
}

所以 input_register_handler() 的作用仅仅是是将这个 evdev_handler 添加到链表 input_handler_list 的尾端。

3） input 驱动层
late_initcall(tpd_device_init);
这里会注册一个 input_dev: input_register_device(tpd->dev);

int input_register_device(struct input_dev *dev)
{
	......
	list_add_tail(&dev->node, &input_dev_list);		
	//将这个 input_dev 添加到 input_dev_list 链表

	list_for_each_entry(handler, &input_handler_list, node)				
		input_attach_handler(dev, handler);
	//遍历 input_handler_list 上的每一个 handler ，调用 input_attach_handler().
	//这里是 input_dev 主动与 handler 进行连接，由于此时 input_handler_list 上面挂载了一个 handler ，所以在这里， handler 和 input_dev 连接成功，调用 evdev_connect() 函数。
	......	
}

在 evdev_connect() 函数中：
调用 input_register_handle() 函数注册一个 input_handle 。
注册一个 cdev ，提供 open/ioctl 供上层应用调用。

input_handle 是连接 input_dev 和 input_handler 的桥梁。只有当 input_dev 和 input_handler 连接成功时，才会注册一个 input_handle 结构体，将二者关连。

int input_register_handle(struct input_handle *handle)
{
	......
	list_add_tail_rcu(&handle->d_node, &dev->h_list);  	
	//将 input_handle 添加到 input_dev->h_list.

	list_add_tail_rcu(&handle->h_node, &handler->h_list);				
    //将 input_handle 添加到 input_handler->h_list.
	......	
}

2. input 子系统有4个重要的结构体：
1) input_dev: input 驱动层的结构体；
2) input_handler: 主要提供了一个 connect() 函数，用于 input_dev 和 input_handler 连接。还提供了 event 处理函数。
3) input_handle: 联系 input_dev 和 input_handler 的桥梁，在二者连接成功时，会注册一个 input_handle 。
4) evdev: 包含一个 cdev ，提供给应用程序的接口。

3. 应用程序只会和事件处理层 event_handler 交互，至于下面的 input 核心层和 input 驱动层，应用程序不会关注。
事件处理层 event_handler 才会和核心层和驱动层交互。

4. 关于 input_register_handler 和 input_register_device
0)
input_register_handler() 用于事件处理层注册一个 evdev_handler.
input_register_device() 用于 input 驱动层注册一个 input_dev.

1） input_register_handler() 函数的作用是：
将新注册的 evdev_handler 添加到链表 input_handler_list 的尾端；
遍历 input_dev_list, 对上面的每一个 input_dev, 调用 input_attach_handler().

2） input_register_device() 函数的作用是：
将新注册的 input_dev 添加到链表 input_dev_list 的尾端；
遍历 input_handler_list, 对上面的每一个 evdev_handler, 调用 input_attach_handler().

3) input_attach_handler() 函数的作用是：
调用 input_match_device(), 这个 evdev_handler 的 match 函数为空，并且 id_table.driver_info = 1, 表示 match 所有 input_dev, 所以它没有效果.
调用 handler->connect(handler, dev, id), 即 evdev_connect().

4) evdev_connect() 函数的作用是：
调用 input_register_handle() 注册一个 input_handle.
在结构 evdev 之下注册一个 cdev, 提供 open/read/write/ioctl 给应用程序使用。
调用 device_add(&evdev->dev), 将这个输入设备添加到 bus 总线。

5) input_register_handle() 函数的作用是：
将 input_handle 添加到 input_dev->h_list.
将 input_handle 添加到 input_handler->h_list.
```



