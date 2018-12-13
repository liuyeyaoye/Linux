〇、关键字

中断，上半部，下半部，软中断，tasklet，工作队列，等待队列。



一、中断基本知识

1、中断是指CPU收到中断处理请求后，停止执行当前程序，转去执行中断程序，执行完中断程序后，返回来被中断的程序处继续执行。

2、中断信号由硬件产生，当外设硬件产生一个中断信号(上升沿/下降沿/高电平/低电平)时，通过对应的中断线传给中断控制器，然后传给CPU。CPU根据是哪一根中断线，转去执行中断向量表中对应的中断处理函数。

3、Linux中断不允许嵌套调用，即中断处理函数在执行的时候，不能被另外的中断给打断。

4、Linux中断分为上半部和下半部，这样的目的是让中断处理足够的迅速，不让其他进程处于饥饿状态。

5、上半部：上半部即中断处理函数，一般执行一些硬件相关的操作如清除中断标志位等，然后调用下半部，然后返回。上半部处理必须迅速。上半部运行在中断上下文，中断上下文没有自己的资源，必须共享当前进程的资源，它也不允许休眠阻塞，不允许被中断，它禁止使用可能引起睡眠的锁如互斥锁mutex_lock。

6、下半部：由上半部调用并且延后执行的一些操作。下半部可以响应中断。下半部的实现方法有软中断，tasklet，工作队列。



二、注册/释放一个中断

1、中断注册函数
```c
int request_irq( unsigned int irq, irq_handler_t handler, unsigned long flags,  
const char *name, void * dev );

typedef irqreturn_t (*irq_handler_t) (int ,void *)  irq_handler_t;

irq：中断线的序号                  
handler：中断处理函数
flags：中断标志，如中断触发方式（上升沿触发或高电平触发），共享中断，快速中断
name：中断名，可以通过 cat /proc/interrupts 看到。                   
dev：传给handler的数据指针

返回值：为0，成功；负值，失败。

request_irq()函数可能会引起睡眠，因为它会调用kmalloc()。
它一般在驱动的probe()函数中调用来注册一个中断号。
```
2、释放中断
```c
void free( unsigned int irq, void * dev );
```


三、软中断

1、软中断运行在下半部和中断上下文。它在编译期间静态的分配，不能动态分配。kernel-4.9使用了10个软中断(10个枚举变量)。

2、软中断的执行：所有的软中断都会被 do_softirq() 调用执行。do_softirq()会调用软中断的执行函数进行处理。

3、如何使用软中断？

(1)首先在枚举变量NR_SOFTIRQS中增加一个新的枚举变量用于软中断名称，例如RCU_SOFTIRQ

(2)注册一个软中断：open_softirq( RCU_SOFTIRQ,  rcu_action ); RCU_SOFTIRQ是软中断枚举变量名，rcu_action 是软中断执行函数。

(3)在中断处理函数中触发软中断：raise_softirq(RCU_SOFTIRQ);  它的执行函数rcu_action 就会在内核下一次调用do_softirq时得到执行。

4、软中断的执行函数要特别注意并发，因为：假如一个软中断执行函数在一个CPU上执行，它又在另一个CPU上被触发然后执行了，这时候，2个相同的软中断就会在2个不同的CPU上同时运行，所以软中断需要严格的锁保护，它一般使用在用于操作单处理器数据(数据仅能用于某个处理器，其他处理器无法使用该数据)。

5、ksoftirqd/n：n表示CPU的序号。每个处理器都有一个辅助处理软中断的内核线程 ksoftirqd 。它的优先级最低，避免跟其他线程抢夺资源。在内核中的软中断特别多的时候，它会被唤醒来处理这些软中断执行函数。它保证了在软中断特别多的时候，用户空间的程序不会得不到处理器而饥饿；在空闲的时候，软中断的处理也特别迅速。



四、tasklet

1、tasklet运行在下半部和中断上下文，它本质上也是软中断的一种，它使用了2个软中断枚举变量来实现自己：HI_SOFTIRQ(0) 和 TASKLET_SOFTIRQ(6)。描述它的数据结构为：
```c
struct tasklet_struct {
    struct tasklet_struct *next;//指向下一个tasklet

    unsigned long state;              
    //0：TASKLET_STATE_SCHED(已经被调度正准备运行)
    //1：TASKLET_STATE_RUN(正在运行)

    atomic_t count;  
    //非0表示tasklet禁止执行；0表示tasklet被激活，允许执行。

    void (*func)(unsigned long);//tasklet执行函数
    unsigned long data; //传给tasklet执行函数的数据
};
```
2、如何使用tasklet？

(1)静态创建一个tasklet：
```c
DECLARE_TASKLET( name, func, data );
//将tasklet的count设为0，允许该tasklet执行

DECLARE_TASKLET_DISABLED( name, func, data );
//跟上面相反的操作

例如：
DECLARE_TASKLET(my_tasklet, tasklet_handler, dev);   
等价于：
struct tasklet_struct my_tasklet{ NULL,0,ATOMIC_INIT(0),tasklet_handler,dev};

也可以动态的创建一个tasklet：
tasklet_init( struct tasklet_struct * t, void(* func)(unsigned long),  
unsigned long data );
```
(2)调度执行tasklet： tasklet_schedule(&my_tasklet);  在tasklet被调度后，只要它有机会，它就会尽快的被执行。

(3)tasklet相关的API
```c
tasklet_disable(&my_tasklet); 
用于禁止某个tasklet，如果这个tasklet的执行函数正在执行，那么这个函数会等待它执行完，
然后禁止这个tasklet。

tasklet_disable_nosync(&my_tasklet); 
非同步禁止tasklet，不会等待这个tasklet执行完，该函数就会返回。

tasklet_enable(&my_tasklet);
使能某个tasklet。

tasklet_kill();
删除某个tasklet，会等待这个某个tasklet执行完再删除。
```


五、工作队列

1、工作队列运行在进程上下文，可休眠，可使用锁，可被中断。

2、内核提供了一组默认的工作者线程 events/n，n表示CPU的序号，每个CPU拥有一个工作者线程，叫做缺省工作者线程。但是如果需要执行大量的操作，可以自己创建一个工作者线程，有助于减少缺省工作者线程的负载，避免缺省工作者线程其他工作处于饥饿状态。

3、描述工作者线程的结构： workqueue_struct 。描述工作的结构：work_struct。

4、如何使用工作队列？

（1）使用缺省的工作队列：
```c
静态的创建一个工作：
DECLARE_WORK(name,  func );

动态的创建一个工作：
static struct work_struct my_work;              
INIT_WORK(&my_work, func );

工作处理函数原型：
void(*func)(struct work_struct work); 
工作处理函数运行在进程上下文，可睡眠，可响应中断。

调度执行工作：
schedule_work(&my_work);              
schedule_delayed_work(&my_work, delay);

刷新：
void flush_schedule_work(void);
等待event/n上所有的工作执行完毕才返回，该函数会引起休眠。

通过schedule_delayed_work()执行的工作，假如它的延迟时间还没有到，那么
flush_schedule_work()不会等待它完成，需要使用cancel_delayed_work()取消延迟工作。
```
（2）自己创建一个工作队列：
```c
在每个CPU都创建工作队列：
struct workqueue_struct create_workqueue(name);

只在当前CPU创建工作队列：
struct workqueue_struct create_singlethread_workqueue(name);

将工作提交到指定的工作队列处理：
int queue_work(struct workqueue_struct * wq, struct work_struct *work);
int queue_delay_work(struct workqueue_struct *wq, struct delayed_work *dwork, 
unsigned long delay);

刷新指定的工作队列：
void flush_workqueue( struct workqueue_struct *wq );

销毁工作队列：
void destroy_workqueue(struct workqueue_struct *wq);
```


六、一些禁止中断的API

1、禁止/使能当前处理器上的所有中断：
```c
local_irq_disable();/local_irq_enable(); 
直接禁止/使能当前CPU上的所有中断。

local_irq_save(flags);/local_irq_restore(flags); 
在禁止中断前保存当前CPU的中断状态(如中断栈信息)，在使能中断后恢复当前CPU的中断状态。
```
2、禁止指定的某一条中断线

（1）API
```c
void disable_irq(unsigned int irq); 
禁止中断线irq上的所有中断，这个函数会等待当前irq中断线上所有中断处理程序完成之后才会返回，
故它会引起阻塞，它一般用在非中断处理函数中。

void disable_irq_nosync(unsigned int irq); 
这个函数是非同步的，不会等待irq线上的中断处理完成就会返回，
它不会引起阻塞，所以它能够用在中断处理函数中。

void enable_irq(unsigned int irq); 
使能某一条中断线的中断
```
（2）为什么要禁止中断线？

当一个中断处理完成之后，然后从中断处理程序返回，这是正常处理的情况。有时候，在中断处理程序正在执行的时候，外设硬件又给中断线发了一个中断，但是此时当前中断还没有完成，这样会造成中断的嵌套，但是Linux禁止中断嵌套。所以，对于某些中断产生频繁的硬件来说(如光感距离感)，在进行中断处理的时候，要禁止中断线，这样才能避免中断嵌套的产生。

（3）注意，不能在中断处理函数中使用disable_irq()，因为它可能引起休眠，而中断处理禁止休眠。

（4）如何使用这些中断线API？

一般这样使用：在中断上半部中断处理函数中 disable_irq_nosync()，禁止中断线，然后调度执行某个工作队列进入中断下半部，然后在工作队列退出之前enable_irq()。

3、禁止/使能当前CPU的下半部中断

local_bh_disable();/local_bh_enable; //禁止/使能当前CPU上的下半部中断(其实是禁止/使能当前CPU上的软中断和tasklet的处理)。

4、与自旋锁的关系

禁止/使能当前处理器上的所有中断 local_irq_disable();/local_irq_enable();  和

禁止/使能当前CPU的下半部中断 local_bh_disable();/local_bh_enable();

这2种函数一般与自旋锁结合使用，因为自旋锁中的代码可能会被中断或者下半部给打断，所以在获取自旋锁之后要禁止中断或下半部，退出自旋锁之后使能中断或下半部：
```c
spin_lock();       
spin_unlock();      
   
local_irq_disable();
local_irq_enable();

local_bh_disable();
local_bh_enable();
```


七、等待队列

1、中断有时候也使用等待队列来实现。

2、等待队列使用例子：
```c
static wait_queue_head_t my_wait_queue_head;
定义一个等待队列头

static atomic_t my_atomic = ATOMIC_INIT(0);
等待队列条件

static task_struct *my_task;
内核线程

request_irq(irq_num,  my_irq_handler, NULL, "my_irq", NULL);
注册中断处理函数

my_task=kthread_create(my_task_func, NULL, "my_task");
创建内核线程

init_waitqueue_head(&my_wait_queue_head);
初始化等待队列头

wake_up_process(my_task);
执行内核线程

static int my_task_func( void *data ) 
{
        wait_event_interruptible( my_wait_queue_head, atomic_read(&my_atomic);
        //线程执行函数休眠在这里
        return 0;
}

static irqreturn_t my_irq_handler( int irq, void * dev) 
{
        atomic_set( &my_atomic,  1);//将等待队列条件置1
        wake_up_interruptible(&my_wait_queue_head);//唤醒休眠在等待队列头上的线程

}
```
3、理解 wait_event_interruptible(wq, condition)

（1）该函数的作用是：将一个线程休眠到等待队列头wq上。

（2）该函数首先会检查condition，如果condition为真，立即返回0，不休眠该线程。

（3）如果condition为假，让出调度器，进入休眠。直到condition变为真或者使用了wake_up_interruptible(wq) ，线程退出休眠，调度执行。



八、面试常问

1、什么时候使用上半部？是么时候使用下半部？

当一个任务对时间敏感、当一个任务跟硬件处理相关、当一个任务要保证不被其他中断给打断，这样的任务，在上半部完成。

其他情况，将它放到下半部执行。

上半部不能响应中断，下半部可以响应中断。

2、什么时候使用软中断、tasklet、工作队列？

软中断必须保证共享数据的安全，因为2个相同的软中断可以在2个不同的处理器上同时运行，对于网络子系统使用单处理器变量，软中断非常适合。对于执行频率高和对时间要求严格的任务，软中断非常适合。

tasklet基于软中断实现，但是tasklet只能在调度它的处理器上执行，这样导致了2个相同的tasklet不能同时执行，在不同的处理器也不能同时执行。驱动程序尽量先考虑使用tasklet，再考虑使用软中断。tasklet会在很短的时间内被执行，并且以原子方式执行。

软中断和tasklet运行在中断上下文，不能休眠，使用锁之前必须禁止中断。工作队列运行在进程上下文，可以休眠，可以使用锁。它们三者都可以响应中断。

3、下半部三种机制软中断、tasklet、工作队列的区别在哪里？

（1）软中断：

运行在中断上下文，不能休眠/阻塞/使用互斥锁mutex，它可以响应中断。软中断执行较快，用于对时间敏感的任务。

2个相同的软中断执行函数可以在2个不同的CPU上同时执行，所以软中断必须严格加锁，所以软中断一般用于处理单处理器数据。

当内核中在某一个时间出现大量软中断待处理时，这时候，内核线程ksoftirqd/n(n表示CPU的序号)就会被唤醒，来处理软中断执行函数，以保证用户空间的程序不被饿死。

软中断的优点在于它的可扩展性，即可以在不同CPU上执行同一软中断，当不需要考虑这个时，使用tasklet。

使用软中断时只能静态定义：定义一个软中断枚举变量，注册软中断处理函数，然后在中断上半部中触发该软中断。退出中断上半部后，软中断就会被内核通过调用函数 do_softirq() 执行。

（2）tasklet：

运行在中断上下文，不能休眠/阻塞/使用互斥锁mutex，它可以响应中断。tasklet执行较快，用于对时间敏感的任务。

tasklet机制是通过软中断来实现的，它使用了2个软中断枚举变量：HI_SOFTIRQ(0)和TASKLET_SOFTIRQ(6)。

1个tasklet只能在调度它的CPU上执行，不能在其他CPU上执行。2个不同的tasklet可以在2个不同的CPU上同时执行。

tasklet的使用方法很简单：首先通过静态DECLARE_TASKLET或者动态tasklet_inti()创建一个tasklet，然后调度执行它即可：tasklet_schedule()。tasklet被调度之后，只要有机会它就会立刻执行。

（3）工作队列：

运行在进程上下文，可以休眠/阻塞/使用互斥锁mutex，它可以响应中断。

内核中有一组缺省的工作者线程events/n(n表示CPU的序号)供我们使用，可以使用内核提供的工作者线程，也可以自己创建单个或者n个工作者线程。

当我们的work比较复杂时，一般自己创建一个工作者线程，避免让内核缺省的工作者线程的其他工作处于饥饿状态。

工作队列的使用方法：如果使用内核缺省的工作者线程events/n：

首先静态定义DECLARE_WORK或者动态定义INIT_WORK一个工作。然后将该工作放到events/n上去执行即可：schedule_work();/schedule_delayed_work;

如果使用自己创建的工作者线程：

创建单个/n个工作者线程：create_singlethread_workqueue();/create_workqueue();

然后提交工作给工作者线程去执行：queue_work();/queue_delayed_work();



九、参考内容
1、Linux kernel development
2、Linux device driver
3、魅族内核团队：http://kernel.meizu.com/linux-interrupt.html
4、蜗窝科技：http://www.wowotech.net/sort/irq_subsystem


