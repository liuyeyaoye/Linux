
```c
platform:mt6761
kernel version:kernel-4.9
Android version:Android 8.1

〇、 base
1. tp 多点触摸
多点触摸协议A和B：
type-A：每次都上报数据；
type-B：B协议需要硬件支持。触点通过slot保存，它用id来标识一个触点，减少触点上报到用户空间的数量。


一、MTK tp driver
1. tp driver 分为2部分：mtk-tpd driver部分和 3rd tp driver部分。
mtk-tpd driver 使用 late_initcall() 注册；
3rd tp driver 使用 module_init() 注册；
mtk-tpd driver 的优先级较低，它用于加载 3rd tp dirver 和 tp driver 兼容处理。

2. 首先进入 3rd tp driver ：
1） 解析dts中的tp信息并保存，如分辨率，虚拟按键值等。
2） 如果使能了tp的距离传感器功能，就要调用 alsps_driver_add() 添加一个距离传感器的driver。
3） 调用 tpd_driver_add() 添加 3rd tp dirver 到 tp driver 数组中保存。

3. 然后是 mtk-tpd driver：
1） 通过工作队列注册 tpd platform driver：
    static struct workqueue_struct *tpd_init_workqueue;
    static struct work_struct tpd_init_work;
    tpd_init_workqueue = create_singlethread_workqueue("mtk-tpd");
    INIT_WORK(&tpd_init_work, tpd_init_work_callback);
    queue_work(tpd_init_workqueue, &tpd_init_work);
工作队列回调函数里面就是创建一个 tpd platform driver。

2） 进入 tpd probe 函数中：
注册一个 misc device，可以对 tp open 和 ioctl，传递给用户空间一些数据；
初始化 tp pinctrl ，复位和中断引脚；
调用 input_allocate_device() 分配一个输入设备，用于触摸输入设备；
设置一些此输入设备支持的事件：EV_ABS/EV_KEY/ABS_X/ABS_Y/ABS_PRESSURE/INPUT_PROP_DIRECT；
遍历 tp driver 数组，调用它的 tpd_local_init() 函数；
--进入 3rd tp driver 的 tpd_local_init() 函数：
--初始化 regulator ，给 tp 上电；
--调用 i2c_add_driver() 添加 tp i2c 驱动；
--进入 tp i2c probe函数，3rd tp 初始化：
----创建工作队列： ts_data->ts_workqueue = create_singlethread_workqueue("fts_wq");
----建立一致性dma映射用于i2c传输数据： dma_alloc_coherent();
----使用协议B，初始化slot： input_mt_init_slots()；
----通过i2c读取 tp ic id和信息；
----创建proc和sysfs文件；
----gesture init：input_set_capability/set_bit，设置支持的手势；
----开启ESD check工作，读寄存器的值是否正确：
	INIT_DELAYED_WORK(&ts_data->esdcheck_work, esdcheck_func);
	queue_delayed_work(ts_data->ts_workqueue, &ts_data->esdcheck_work, msecs_to_jiffies(ESDCHECK_WAIT_TIME));
	在 esdcheck_func 有： queue_delayed_work(ts_data->ts_workqueue, &ts_data->esdcheck_work,msecs_to_jiffies(ESDCHECK_WAIT_TIME)); 一直循环这个工作。
	ESD如果失败就执行复位和recovery。
----创建一个上报触点的线程： 
      ts_data->thread_tpd = kthread_run(touch_event_handler, 0, TPD_DEVICE);
----这个线程一进去就休眠在一个等待队列上，在中断处理函数中被唤醒： 
      wait_event_interruptible(waiter, tpd_flag != 0);
----如果等待队列被唤醒了，才会读取数据并且上报。
----注册中断： request_irq()，tp的中断处理函数唤醒等待队列：
      wake_up_interruptible(&waiter);
----开启tp固件升级任务： 
	INIT_WORK(&ts_data->fwupg_work, fts_fwupg_work);
	queue_work(ts_data->ts_workqueue, &ts_data->fwupg_work);
----退出 3rd dirver probe 函数；
创建 tp resume 工作队列：
    touch_resume_workqueue = create_singlethread_workqueue("touch_resume");
    INIT_WORK(&touch_resume_work, touch_resume_workqueue_callback);
创建内核通知链，用于tp resume事件的处理：
    tpd_fb_notifier.notifier_call = tpd_fb_notifier_callback;
    fb_register_client(&tpd_fb_notifier);
如果收到了LCD亮的事件，调用：
    queue_work(touch_resume_workqueue,&touch_resume_work);
    在这个工作的 callback 中调用 tp 的 resume 函数；
如果收到了LCD灭的事件，调用：
    g_tpd_drv->suspend(NULL);
这样就不会让tp的resume卡LCD亮屏时间，但是在LCD亮屏一瞬间tp无法使用。
调用set_bit()设置该输入设备支持的事件：如 ABS_MT_TRACKING_ID；
注册输入设备：input_register_device(tpd->dev)；
如果有虚拟按键，调用：tpd_button_init()，在这个函数里面为虚拟按键分配和注册一个输入设备：input_allocate_device，input_register_device；
为设备创建sysfs属性文件，device_create_file；

3. tp 驱动使用了中断上半部和下半部、内核线程、工作队列、等待队列：
1） 
tp 的上报按键处理在一个内核线程中进行，这个内核线程一开始就休眠在一个等待队列上。
在 tp 中断处理函数中(中断上半部)，唤醒等待队列，然后执行内核线程(中断下半部)。
2） tp 驱动创建了2个工作队列：
第1个工作队列有2个工作：
ESD check 工作，这是个循环的工作，一直都会执行；
tp 固件升级工作，这个工作只执行一次；
第2个工作队列有1个工作，用于 tp resume:
这里还使用了内核通知链：
在收到LCD亮的通知后，将 tp resume 工作添加到工作队列，这个工作稍后将会得到执行；
在收到LCD灭的通知后，直接执行 tp suspend 函数；
```

