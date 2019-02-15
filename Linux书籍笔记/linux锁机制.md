


〇、关键字

锁，原子操作，自旋锁，信号量，互斥体，完成量。


一、锁的基本知识

1、锁是通过原子操作实现的，它本身不会导致竞态。

2、为什么要使用锁？

答：因为进程同步的存在，所以要使用锁。

用户空间：

（1）用户程序会被调度器抢占和重新调度，用户程序可以在任何时刻被抢占，调度器会选择一个高优先级的进程到处理器上执行。

（2）SMP的存在使得多个处理器同时执行代码。

内核空间：

中断：中断处理程序可能随时打断当前正在执行的代码。

软中断和tasklet：内核可能随时调度执行软中断和tasklet的代码，打断当前正在执行的代码。

内核抢占：内核可抢占，高优先级可以抢占低优先级的代码。

睡眠：内核的代码可能会睡眠，这样调度器会调度其他代码执行。

对称多处理器：多个处理器同时执行代码。

3、死锁

（1）死锁的种类：

自死锁：一个线程获取了某个锁，然后它再试图去获取这个锁，造成死锁。

ABBA锁：线程1获取了A锁，然后它试图获取B锁；线程2获取了B锁，然后它试图获取A锁。

（2）怎么预防死锁？

按顺序上锁，按相反顺序解锁。

不要重复获取同一个锁。

尽量使用简单一点的锁方案。

4、使用锁机制有什么缺点？

当多个线程同时请求同一个锁时，这个锁处于高度争用的状态，会造成系统性能的下降。解决方法是重新选择锁方案，不让多个线程争用同一个锁。



二、内核同步方法

1、原子操作

（1）内核提供了2种原子操作：一种对整数进行原子操作，一种对位进行原子操作。原子操作的实现跟系统架构相关。原子操作常用于计数器。

（2）为什么使用 atomic_t 类型来定义原子变量？

第一，这个类型专门提供给原子操作，以区分原子操作和非原子操作。

第二，它可以屏蔽体系架构的差异，增强代码移植性。

第三，编译器不会对 atomic_t 类型的变量进行优化。
```c
typedef struct { 
    int counter;
} atomic_t;
```
（3）原子操作API
```c
ATOMIC_INIT( int i );            
int atomic_read( atomic_t * v );
void atomic_set( atomic_t  *v, int i );
void atomic_add( int i, atomic_t * v );
void atomic_sub( int i, atomic_t * v );
void atomic_inc(atomic_t * v);
void atomic_dec(atomic_t * v);

int atomic_sub_and_test(int i, atomic_t *v);/
将v减去i，如果等于0则返回真，否则返回假。

int atomic_add_return(int i, atomic_t *v);
将v加上i，返回

int atomic_sub_return(int i, atomic_t *v);;
将v减去i，返回

int atomic_inc_return(atomic_t *v);
将v加1，返回

int atomic_dec_return(atomic_t *v);
将v减1，返回

int atomic_inc_and_test(atomic_t *v);
将v加1，如果等于0则返回真，否则返回假

int atomic_dec_and_test(atomic_t *v);
将v减1，如果等于0则返回真，否则返回假
```
（4）原子操作与顺序操作有什么不同？

原子操作保证当前的操作原子完成，即要么全部完成，要么全部不完成，决不会被打断；

顺序操作表示2个操作必须按顺序完成，前后的操作必须在后面的操作之前完成，不能被编译器和处理器给优化了，顺序操作通过屏障barrier来完成。

（5）64位原子操作：用在特殊架构和需要64位原子变量的代码中
```c
typedef struct {
    long count;
}atomic64_t;
```
（6）原子位操作API
```c
void set_bit(int nr, void * addr);
原子的设置addr的nr位为1

void clear_bit(int nr, void * addr);
原子的清除addr的nr位

void change_bit(int nr, void * addr);
原子的翻转addr的nr位

int test_and_set_bit(int nr, void *addr);
原子的设置addr的nr位并且返回原先的nr位的值

int test_and_clear_bit(int nr,void *addr);
原子的清除addr的nr位并且返回原先的nr位的值

int test_and_change_bit(int nr, void*addr);
原子的翻转addr的nr位并且返回原先的nr位的值

int test_bit(int nr , void *addr);
原子的返回addr的nr位
```
内核还提供了与上面操作相对应的非原子操作，在上面那些函数前面加2个下划线，例如：__set_bit(int nr,void *addr);如果你已经使用了锁保护，那么可以使用非原子操作。



2、自旋锁

（1）自旋锁最多被一个线程持有，如果其他线程试图获取一个已经被持有的自旋锁，那么它必须一直等待，一直占用CPU等待这个自旋锁可用为止。

自旋锁不应该被长期持有，它一般用来锁定耗时较少的代码，比如修改一个全局变量的值。自旋锁里的代码禁止休眠(避免其他想获取这个自旋锁的线程等待太长时间或者死锁)。

自旋锁可以在中断上下文使用（信号量不能使用在中断中，因为它可能会休眠）。在中断中使用自旋锁时，必须首先禁止本地(当前处理器)中断，否则中断可能打断自旋锁里的代码，有可能试图去争取这个自旋锁，引起中断死锁。

如果一个线程的自旋锁可能在中断处理程序中使用，那么使用它之前必须禁止本地中断。使用函数：
```c
spin_lock_irq();
spin_unlock_irq();
spin_lock_irqsave();
spin_unlock_irqrestore();
```
如果一个线程的自旋锁可能在软中断和tasklet中使用，那么使用它之前必须禁止中断下半部。使用函数：
```c
spin_lock_bh();
spin_unlock_bh();
```

如果一个线程的自旋锁不会在中断和下半部用到，则不必禁止中断或者下半部。

Linux中的自旋锁不允许递归调用。

（2）使用自旋锁

自旋锁的实现跟具体的体系结构密切相关，它一般使用汇编代码来实现：
```c
DEFINE_SPINLOCK(my_lock);
定义一个自旋锁    

spin_lock(&my_lock);
获取自旋锁

spin_unlock(&my_lock);
释放自旋锁

spin_lock_irqsave(&my_lock, flags);
中断处于未激活的状态，需要保存当前的中断状态，禁止本地中断，获取锁。

spin_unlock_irqrestore(&my_lock, flags);
释放锁，中断处于未激活的状态，需要恢复当前的中断状态，使能本地中断。

如果能够确定中断在加锁前是激活的，那么不需要保存和恢复中断的状态：
spin_lock_irq(&my_lock);
spin_unlock_irq(&my_lock);

如果自旋锁可能在软中断和tasklet中使用，必须禁止/使能本地中断下半部：
spin_lock_bh();
spin_unlock_bh();
```
（3）调试自旋锁：打开宏CONFIG_DEBUG_SPINLOCK和CONFIG_DEBUG_LOCK_ALLOC

（4）其他API
```c
spin_lock_init();
动态的初始化指定的自旋锁

spin_trylock();
试图获取自旋锁，如果获取不到，立即返回非0

spin_is_locked();
如果指定的自旋锁被占用，返回非0
```
（5）使用自旋锁的方法

如果进程上下文和中断共享了自旋锁，那么在进程上下文使用自旋锁前要禁止中断。

如果中断上半部和下半部共享了自旋锁，那么在下半部(软中断和tasklet)使用自旋锁前要禁止中断。

在中断下半部(软中断和tasklet)中，如果要使用自旋锁，可以不必禁止下半部。这是因为：同一处理器的软中断不会抢占另一个软中断。在同一处理器上不会出现tasklet相互抢占的情形。

（6）读写自旋锁

一个或多个任务可以并发的持有读锁，但是写锁只能被一个任务持有，在写入的时候禁止读。
```c
DEFINE_RWLOCK(my_rwlock);

读：
read_lock(&my_rwlock);
read_unlock(&my_rwlock);

read_lock_irq();
read_unlock_irq();

read_lock_irqsave();
read_unlock_irqsave();

写：
write_lock(&my_rwlock);
write_unlock(&my_rwlock);

write_lock_irq();
write_unlock_irq();

write_lock_irqsave();
write_unlock_irqsave();

write_trylock();
尝试获取指定的写锁，如果写锁不可用，返回非0

rwlock_init();
初始化指定的rwlock_t
```


3、信号量
（1）理解信号量

当一个线程请求的信号量正在被其他线程使用时，这个线程就会被休眠到一个等待队列上，它会释放CPU，CPU可以去处理其他的事情。当这个信号量被其他线程释放了，休眠在等待队列的线程会被唤醒。这样处理带来了一定的开销，有2次明显的线程上下文切换，被阻塞的线程要换入和换出。

Linux中的信号量是一种睡眠锁，它比自旋锁具有更大的开销。

由于争用信号量的进程可能会被休眠，所以信号量适用于锁被长时间持有并且可能导致休眠的代码，例如使用 pinctrl_select_state()时就需要使用互斥体（等同于计数为1的信号量）来锁定。

当锁被短时间持有时，不适合使用信号量，因为信号量带来的进程上下文的开销比信号量的持有时间还要长，此时应该使用自旋锁。

由于持有信号量允许睡眠，所以它不能用在中断上下文中，只能用在进程上下文中。因为中断不允许睡眠，不允许进行调度。

持有信号量的进程允许去睡眠，因为另外一个争夺该信号量的进程会被睡眠，不会导致死锁。

信号量不会禁止内核抢占，所有持有信号量的代码都可以被高优先级的进程抢占。

当信号量的计数值为1的时候，它称为互斥信号量，用于多个进程的互斥。当信号量的计数值不为1时，称为计数信号量，主要用于多个进程的同步（有序执行）。

（2）信号量API
```c
down();
一个线程试图获取一个信号量，该函数会对信号量计数值减1。如果计数值大于或等于0，该线程得到信号量。
否则，该线程被置为 TASK_UNINTERRUPTIBEL 的状态，进入一个等待队列休眠。

down_interruptible();
跟上面类似，如果线程获取不到信号量，它的状态被置为 TASK_INTERRUPTIBLE ，然后进入等待队列休眠。
如果在休眠的过程中这个线程收到了一个信号，那么它可以被信号唤醒退出休眠。

down_trylock();
如果获取不到信号量，立即返回非0。

up();
释放信号量，将计数值加1。

struct semaphore name;
定义一个信号量

sem_init(&name, count);
初始化一个信号量，初始化它的计数值为count

static DECLARE_MUTEX(name);
定义一个信号量，初始化它的计数值为1

如果sem是一个信号量/互斥信号量的指针，可以这样初始化它：
sem_init(sem, count);
init_MUTEX(sem);//计数值为1
```
（3）读写信号量
```c
static DECLARE_RWSEM(name);
init_rwsem(struct rw_semaphore *sem);
```
所有的读写信号量都是互斥信号量，计数置为1。它只对写入者互斥，不对读者互斥。只要没有写者，并发持有读锁的读者不限。所有读写锁都不会被信号打断，所以它只有一个版本的down()操作。
```c
down_read();
up_read();
down_write();
up_write();
down_read_trylock();
down_write_trylock();
```


4、互斥体mutex

（1）信号量适用于复杂的上锁情况，比如多个进程之间的同步处理、用户和内核复杂的交互行为。但是在内核中，信号量使用不多，一般使用内核的互斥体mutex。互斥体指的是可睡眠的锁，比如计数值为1的信号量。互斥体和信号量不同，它等价于计数值为1的信号量，但是二者实现方式不同。

任何时候只能有一个线程持有mutex，即mutex的计数值为1。

一般在同一个上下文使用mutex来上锁和解锁。mutex不适用于内核复杂的多进程同步。mutex不允许递归的调用。当持有一个mutex时，线程不能在未释放mutex之前就退出执行。mutex不能用在中断上半部和下半部。

打开宏CONFIG_DEBUG_MUTEXES用来调试mutex。

（2）使用互斥体
```c
DEFINE_MUTEX(name);
mutex_init(&lock);
mutex_lock(&lock);
mutex_unlock(&lock);
mutex_trylock(struct mutex*);
mutex_is_locked(struct mutex*);
```
（3）自旋锁和互斥体的使用场景

低开销上锁：使用自旋锁

短期锁定：使用自旋锁

长期上锁：使用互斥体

中断上下文加锁：使用自旋锁

持有锁的线程需要睡眠：使用互斥体

只有当互斥体解决不了时，才使用信号量。否则使用互斥体。



5、完成量

（1）完成量用于：在内核中，一个线程通知另外一个线程自己完成了某件事，给另外一个线程发送一个完成信号。完成量用于2个线程的同步。

例如：线程1要完成一些工作时，线程2就在完成量上等待。当线程1完成了工作时，给线程2发送一个完成量信号，线程2收到信号后继续执行。

（2）使用完成量
```c
DECLARE_COMPLETION(name);
init_completion(struct completion*);
```
在一个任务上，调用 wait_for_completion(struct completion*)来等待特定事件。当特定事件发生后，产生事件的任务调用 complete(struct completion*) 来唤醒正在等待的任务。



6、BLK大内核锁

（1）大内核锁已经过时，它是一个粗粒度锁机制。使用方法为：
```c
lock_kernel();
unlock_kernel();
```


7、顺序锁（seq锁）

（1）适应的情况：数据存在很多读者；写者很少。写优先于读，不允许读者让写者饥饿。数据很简单。jiffies使用seq锁来读写。

（2）使用方法
```c
seqlock_t name = DEFINE_SEQLOCK(name);

写锁：
write_seqlock(&name);
write_sequnlock(&name);

读锁：
unsigned long seq;
do {
    seq = read_seqbegin(&name);
    ...
} while(read_seqretry(&name, seq)) ;
```


8、禁止抢占

（1）Linux内核是可抢占的，内核中的进程在任何时刻都可能停下来以便另一个具有更高优先权的进程执行。

（2）禁止内核抢占：可以嵌套调用它，禁止和使能必须一一对应。
```c
preempt_disable();
//增加抢占计数值，从而禁止内核抢占

...

preempt_enable();
//减少抢占计数，当该值将为0时检查和执行被挂起的需要调度的任务

preempt_count();
//返回抢占计数
```


9、顺序和屏障

（1）在和硬件交互时，经常要保证一个给定的读操作在其他读/写操作之前完成。另外在多处理器上，可能需要按写数据的顺序读数据。但是编译器和处理器为了提高效率，可能对读写操作重新排序。

（2）使用方法

rmb() :读内存屏障，它能够保证在rmb()之前的载入操作不会被重新排在该调用之后，同样，在rmb()之后的载入操作不会被重新排在该调用之前。

wmb() :写内存屏障，它跟上面类似，它只能针对存储而非载入，它确保跨越屏障的存储不发生重新排序。

mb()  ：读写内存屏障，它确保了载入操作和存储操作都不会跨越屏障重新排序。

barrier() :防止编译器跨屏障对载入和存储操作进行优化，防止编译器可能重新排序指令。



10、 RCU（读取-复制-更新）

RCU针对经常发生读取而很少写入的情况。在需要修改某个数据结构时，写入线程首先复制，然后修改副本，然后用新的版本替代相关指针。在内核确认老的版本没有其他引用时，就可释放老的版本。



参考内容：

1、Linux内核设计与实现

2、Linux设备驱动程序


