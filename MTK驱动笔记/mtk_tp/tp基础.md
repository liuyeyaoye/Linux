
```c
TP 基础知识：

一、 MTK TP代码架构
1、MTK的TP分为MTK TP 和 第三方TP驱动。

2、TP代码的工作队列、中断、多线程
1）MTK TP 的工作队列：
1.1）  init workqueue ： 里面有一个 work ，platform driver register ，这个 work 直接执行：queue_work()

1.2） tp resume workqueue ： 里面有一个 work ：它的工作内容是调用 tp->resume() 。它在LCD亮屏时通过内核通知链被执行 queue_work() 。（TP 的 resume 和 suspend 都是通过LCD内核通知链被调用。）


2）ft8006m 的工作队列：它有1个工作队列，有3个工作： fwupd ， esd ， point_report_check 。其中 ，esd work 将每隔1s就会执行一次。

3）TP中有一个内核线程：touch_event_handler ，它通过等待队列和中断来实现，线程执行函数等待中断发生，休眠在一个等待队列头上。当发生了中断，唤醒等待队列头上的这个线程。

4) TP 中的多线程有2个：esd 工作 和 中断处理底半部处理线程。（中断上半部即 request_irq 的中断函数，仅仅是唤醒了等待队列头上的中断底半部线程。中断底半部上报按键和绝对坐标）

5）中断一般与等待队列结合使用。工作队列可以和内核线程相提并论。


2、 
set_bit()	告诉input输入子系统支持哪些事件，哪些按键。
input_set_abs_params()		设置参数范围。

3、 MTK平台GPIO操作的三种方法：
使用GPIOlib ： gpio_request()   gpio_direct_output()	gpio_set_value()	 gpio_free()
使用pinctrl ：	 devm_pinctrl_get()	pinctrl_lookup_state()		pinctrl_select_state()
使用legacy mtk 函数： mt_set_gpio_mode()   mt_set_gpio_dir()


```

