一、关键字
内存地址、内存分配函数、slab、DMA



二、内存基础知识

1、页

内核通常把页作为内存管理的基本单元，尽管处理器的最小可寻址单位为字或者字节，但是，MMU（管理内存并把虚拟地址转换为物理地址的硬件）通常以页为单位进行管理。大多数32位系统的页大小为4KB，64位系统的页大小为8KB。

2、区

（1）由于硬件的限制，内核中的页不是完全相同的，有的页位于特定的物理地址上，所以它不能用于一些特定的任务。由于这些限制，内核把页分为了不同的区（zone）。区是具有相同特性的页的集合。

（2）Linux必须处理2种由于硬件缺陷而导致的内存寻址问题：
一些硬件只能在特殊的内存上进行直接内存访问（DMA）操作；
一些体系架构的物理寻址范围比虚拟寻址范围大得多，导致一些内存不能永久的映射到内核空间；

（3）Linux内存分为3个区：

常规内存区：这个区包含正常映射的页。
DMA内存区：这个区包含的页用来执行DMA操作。
高端内存区：其中的页不能永久的映射到内核地址空间，高端内存在内核地址空间没有对应不变的内存。


3、高端内存和低端内存
低端内存：在内核空间存在逻辑地址，与它的虚拟地址对应。
高端内存：不存在逻辑地址，只有虚拟地址。

（1）32位系统中，内存的最大寻址空间为4GB，内核将4GB的虚拟地址分为2部分。用户空间为3GB，内核空间为1GB。占用内核空间最大的部分是物理地址的虚拟映射，内核不能操作没有映射到内核空间地址的内存。内核对任何内存的访问，都需要使用自己的虚拟地址。

（2）有的处理器支持“地址拓展”特性，即32位系统可以在超过4GB空间之外进行寻址，但是有多少内存可以映射到内核逻辑地址的限制仍然存在。

（3）只有内核的低端内存（1GB—2GB）拥有它们的内核逻辑地址。剩余的部分（高端内存）是没有内核逻辑地址的，但是有物理地址。在访问高端内存之前，必须在内核中建立明确的虚拟映射，使该页可以在内核空间被访问。

（4）许多内核的数据结构都放在低端内存，而用户空间的进程页一般放在高端内存。



4、地址类型
Linux是一个虚拟内存系统，用户空间使用的内存地址跟实际的物理内存地址是不一样的。有了虚拟内存，在系统中运行的程序可以分配比物理内存更多可以使用的内存空间。

（1）用户虚拟地址：这是用户程序看到的地址空间，为32位或64位，每个进程都有自己的虚拟地址空间。

（2）物理地址：该地址在处理器和内存之间使用，是32位或64位的。

（3）总线地址：该地址在外围总线和内存之间使用，一般跟物理地址相同。

（4）内核逻辑地址：这是内核的常规内存空间，该地址映射了部分或者全部的内存，并经常被视为物理内存。逻辑地址和其相关联的物理地址的不同之处，仅仅在于它们之间存在一个固定的偏移量。逻辑地址使用硬件内建的指针大小，因此在安装了大量内存的32位系统中，逻辑地址无法寻址全部的物理地址。逻辑地址保存在unsigned long 或者 void *指针中。kmalloc()返回的就是逻辑地址。

宏__pa()可以将逻辑地址转换为一个物理地址；

宏__va()可以将一个物理地址转换位一个逻辑地址。

（5）内核虚拟地址；

内核虚拟地址与逻辑地址的相同之处：它们都将内核空间的地址映射到物理地址上。内核虚拟地址与物理地址的映射不必是一对一的，但是内核逻辑地址与物理地址的映射必须是一对一的。所有的内核逻辑地址都是内核虚拟地址，但是许多内核虚拟地址却不是内核逻辑地址。vmalloc()返回的就是内核虚拟地址，它不存在直接的物理映射。kmap()返回的也是内核虚拟地址。



三、分配页的相关函数

（1）分配1个页
```c
struct page *alloc_page( gfp_t gfp_mask );
//该函数分配1页，返回第一个物理页的page指针(虚拟地址)。

unsigned long __get_free_page( gfp_t gfp_mask );
//该函数分配1页，返回第一个物理页的逻辑地址。

unsigned long get_zeroed_page( gfp_t gfp_mask );
//该函数分配1页，让其内容填充为0，返回第一个物理页的逻辑地址。
```
（2）分配2的order次幂个页
```c
struct page* alloc_pages( gfp_t gfp_mask,  unsigned int order );
//该函数分配(1<<order)个连续的物理页，返回第一个物理页的page指针(虚拟地址)。
//只有alloc_pages()才能分配高端内存。

unsigned long __get_free_pages( gfp_t gfp_mask,  unsigned int order );
//该函数分配(1<<order)个连续的物理页，返回第一个物理页的逻辑地址。
//因为页是连续的，所以其他页紧随其后。
```
（3）释放页
```c
void __free_page(struct page *page);

void __free_pages(struct page *page, unsigned int order);

void free_page(unsigned long addr);

void free_pages(unsigned long addr, unsigned int order);
```
（4）其他函数
```c
void * page_address( struct page * page);//该函数返回指向给定的物理页的逻辑地址。
```
（5）gfp_mask标志

常用的修饰符有：
```c
区修饰符：

__GFP_DMA ：从 ZONE_DMA 分配内存
__GFP_DMA32：  从 ZONE_DMA32 分配内存
__GFP_HIGHMEM： 从 ZONE_HIGHMEM 或 ZONE_NORMAL 分配内存

不能给 __get_free_pages() 或 kmalloc() 指定 __GFP_HIGHMEM 标志，
因为这2个函数的返回的都是逻辑地址，而不是page结构。只有 alloc_pages() 才能分配高端内存。
```
类型标志：
```c
GFP_ATOMIC  ：这个表示用在中断上半部、下半部、持有自旋锁的代码中以及其他不能睡眠的地方。
GFP_KERNEL ：常规内存分配方式，可能会阻塞，用在进程上下文。
GFP_DMA ：从 ZONE_DMA 进行分配。
```


四、分配内存和内存映射相关的函数

（1）kmalloc
```c
void * kmalloc( size_t size, gfp_t gfp_mask );
该函数分配size个字节，它分配的内存在物理上一定是连续的，所以虚拟地址也是连续的。它可能睡眠。

void kfree(const void * ptr);
该函数释放由kmalloc()分配的内存。
```
（2）vmalloc
```c
void * vmalloc( unsigned long size);
该函数分配size个字节，它可能睡眠。
它一般用于大内存分配，它分配的内存在物理上不是连续的，所以需要重新建立页表。
使用它会影响系统的性能。它可用在为将要被insmode的一个内核模块分配内存。

void vfree(const void *addr);
该函数释放由vmalloc分配的内存，它可能睡眠。
```
（3）ioremap
```c
void *ioremap(unsigned long offset, unsigned long size);
该函数是内核提供的用来映射外设寄存器的，它将一个I/O空间映射到内核虚拟地址空间,
然后内核就可以对它进行操作。它会建立新的页表，返回一个虚拟地址，用来访问指定的物理地址。

void iounmap(void *addr);
该函数释放ioremap()映射的寄存器地址。
```
（4）mmap()系统调用
```c
void *mmap(void *start,size_t len,   int port,int flags,   int fd,   off_t offset  );
该系统调用是为用户程序提供的，它可以直接映射一段内存给用户程序使用，
然后用户程序可以直接访问这段内存，相较于在用户空间和内核空间拷贝数据，效率较高。
在要求高性能的设备中它比较常用，mmap映射内存必须是页大小的整数倍。mmap的实现跟硬件有关。

int munmap(void *start, size_t length);
释放由mmap()映射的内存。
```
（5）kmalloc()和vmalloc()的区别

vmalloc：分配的内存虚拟地址是连续的，但是物理地址不是连续的，它类似于用户空间分配内存的函数malloc()。

kmalloc：分配的内存虚拟地址是连续的，物理地址也是连续的。

大多数情况下，只有硬件设备需要分配物理上连续的内存，硬件设备用到的内存必须是物理上页连续的内存。而供软件使用的内存块，如进程缓冲区，只需要虚拟内存是连续的就行了。

尽管只有硬件需要分配物理上连续的内存，软件使用的不需要物理上连续的内存，但是，在内核中分配内存一般都用kmalloc()。这是出于性能的考虑。vmalloc()分配内存时，必须建立专门的页表项，对分配的页表进行一个一个的映射(因为它们的物理内存不连续)，这相较于直接内存映射会产生较大的TLB（一种硬件缓冲区，用来缓存物理内存和虚拟内存的映射关系，极大的提高系统性能）抖动。所以一般不会使用vmalloc()，在获取大内存时，例如为一个插入内核的模块分配内存时，才会使用vmalloc来分配内存。

而kmalloc和__get_free_pages()分配的内存不需要修改页表，它们的虚拟地址跟物理地址都是一一对应的，可能会有一个偏移PAGE_OFFSET。

（6）高端内存的映射

高端内存只能通过函数 alloc_pages()来分配，它的页不能永久的或者自动的映射到内核地址空间上，因此通过alloc_pages()函数以__GFP_HIGHMEM 标志分配的内存不可能有逻辑地址。

永久映射：要映射一个给定的page到内核地址空间，使用函数：void *kmap(struct page *page);

kmap()在低端内存和高端内存都能使用，它可能睡眠。当page对应的是低端内存的一页时，它会返回该页的虚拟地址。当page对应的是高端内存的一页时，则会建立一个永久映射然后返回地址。

永久映射的数量是有限的（如果没有这个限制，直接把所有的内存都映射到内核空间就行了），所以当不需要高端内存时，应当解除映射：void kunmap(struct page *page);

临时映射：当需要映射高端内存但是不能睡眠时，使用函数：
```c
void * kmap_atomic(struct page *page, enum km_type type);
```

这个函数不会引起睡眠，它还会禁止内核抢占，可以把它用在中断或者其他不允许睡眠的地方。

取消临时映射：void kunmap_atomic(void *kvaddr, enum km_type type);它也不会睡眠。

（7）分配函数的选择

如果要使用连续的物理页，使用某个低级的页分配函数或者kmalloc()，它常用的标志有GFP_ATOMIC 和 GFP_KERNEL，前者不会睡眠，后者可能会引起睡眠。

如果需要分配高端内存，使用alloc_pages()函数，它的返回值为struct page指针，访问该高端内存唯一的方式是使用struct page指针。如果想要使用真正的地址，使用kmap()函数进行映射，它的返回值为void *，即内核逻辑地址。

如果不需要物理上连续的页，只需要虚拟地址连续的页就行，使用vmalloc()函数，它用于大内存分配，它会导致内核性能损失，它仅仅把物理页映射为连续的虚拟地址。

如果要分配和撤销许多大的数据结构，使用slab分配器。slab层为每个处理器分配一个高速缓存，高速缓存能够提高系统分配和释放数据结构的性能，它不会频繁的分配和释放内存。当需要分配一个数据结构时，从高速缓存中取一个使用就行。



五、slab分配器

1、为什么需要slab分配器

Linux内存管理中分配内存的最小单位为PAGE_SIZE，一页为4KB，但是很多情况下我们不需要这个大的内存，会产生浪费。slab就是一种细粒度分配内存机制。

2、kmalloc也是基于slab实现的。

3、slab分配器原则：

频繁使用的数据结构体也会频繁的释放，使用slab分配。

频繁的分配和释放会导致内存碎片，slab分配不会。

slab分配时，被回收的对象可以在下一次分配时立即投入使用，提高系统性能。

如果让部分缓存专属于pre-CPU所有，那么使用它不需要加锁。

4、slab把不同类型的对象分为不同的高速缓存组，比如一个高速缓存保存的数据结构是struct task_struct，另一个高速缓存保存的数据结构体是struct inode类型。kmalloc也通过一组高速缓存来实现。然后这些高速缓存被划分为多个slab，每个slab由一个页或者多个连续的物理页组成，每个slab保存了尽可能多的对象。

每个slab处于三种状态：满，部分满，空。如果内核需要一个对象时，先从部分满的slab进行分配，如果没有部分满的slab，就从空的slab进行分配。如果没有空的slab，就要创建一个新的slab。这样可以减少内存碎片化。

5、slab结构体：
```c
struct slab {
    struct list_haed list;
    unsigned long colouroff;
    void *s_mem;
    unsigned int inuse;
    kmem_bufctl_t free;
};
```
6、创建一个新的高速缓存
```c
struct kmem_cache * kmem_cache_create(const char *name, size_t size, size_t align, 
unsigned long flags,   void (*ctor)(void*)  );

name:高速缓存的名字
size：高速缓存中每个元素的大小
align:slab内第一个对象的偏移
```
flags:可以为0，表示没有特殊的行为。它可以为：

SLAB_HWCACHE_ALIGN:把一个slab内的所有对象按高速缓存行对齐。这样可以提高性能，但以增加内存开销为代价。对齐越严格，浪费的内存越多。到底会浪费多少内存，取决于对象的大小以及对象相对于系统高速缓存行对齐的方式。对于频繁使用的高速缓存并且代码本身对性能要求严格的，可以配置它。

SLAB_POISON:使slab用已知的值（a5a5a5a5）填充slab，有利于对未初始化内存的访问。

SLAB_RED_ZONE:在已分配的内存周围插入 RED ZONE，以探测缓冲区越界。

SLAB_PANIC:这个标志当分配失败时提醒slab层，这在要求分配只能成功的时候非常有用。

SLAB_CACHE_DMA：从DMA的区段分配内存给slab分配器。

ctor:当新的页追加到高速缓存时，这个函数会被调用。Linux内核的高速缓存不需要这个构造函数，可以置为NULL。

kmem_cache_create()在成功时返回一个指向它所创建高速缓存的指针，否则返回NULL。它可能睡眠。

7、销毁一个高速缓存
```c
int kmem_cache_destroy(struct kmem_cache *cachep);
```
撤销一个高速缓存。它可能睡眠。调用它之前必须满足2个条件：

高速缓存的所有slab都必须为空。

调用 kmem_cache_destroy() 的过程中，不能再访问这个高速缓存，调用者必须确保这种同步。

它在成功时返回0，否则返回非0.

8、从高速缓存分配一个对象
```c
void * kmem_cache_alloc(struct kmem_cache * cachep, gfp_t flags);
```
该函数从给定的高速缓存 cachep 中返回一个指向对象的指针。如果高速缓存的使用slab中都没有空闲的对象，那么slab层必须通过 kmem_getpages() 获取新的页，

flags 的值传递给 __get_free_pages 。

9、创建/释放一个slab
```c
创建新的slab：  
void * kmem_getpages(struct kmem_cache * cachep, gfp_t flags, int nodeid);

释放slab：    
void kmem_freepages(struct kmem_cache *cachep, struct page *page);
```
10、slab 使用实例：
```c
//创建一个高速缓存，对象为 struct task_struct
struct kmem_cache *task_struct_cachep;
task_struct_cachep = kmem_cache_create("task_struct",sizeof(struct task_struct),
ARCH_MIN_TASKALIGN,     SLAB_PANIC | SLAB_NOTRACK,     NULL  );

//从高速缓存中获取一个进程描述符对象
struct task_struct *tsk;
tsk = kmem_cache_alloc(task_struct_cachep, GFP_KERNEL);

//释放进程描述符对象，将它返回给高速缓存
kmem_cache_free(task_struct_cachep, tsk);

//撤销进程高速缓存
kmem_cache_destroy(task_struct_cachep);
```


六、内存池

内核中有些地方时不允许内存分配失败的，在这种情况下使用 mempool 来分配内存。 mempool 试图始终保存空闲的内存，以便在紧急情况想使用。
```c
mempool_t * mempool_create( int min_nr,       mempool_alloc_t *alloc_fn, 
mempool_free_t *free_fn,   void *pool_data    );

min_nr:表示内存池始终保持的已分配对象的最少数据。对象的实际分配和释放由 alloc_fn 和 free_fn 处理。
typedef void * (mempool_alloc_t) (int gfp_mask, void * pool_data);
typedef void (mempool_free_t) (void *element, void *pool_data);
```
通常让内核的slab分配器来处理内存池，示例代码：
```c
cache = kmem_cache_create(...);
pool = mempool_create(MY_POOL_MINNUM,  mempool_alloc_slab,  mempool_free_slab,  cache);

在建立内存池后，使用下面的函数分配和释放对象：
void * mempool_alloc(mempool_t *pool, int gfp_mask);
void mempool_free(void * element, mempool_t *pool);

调整mempool的大小：    
int mempool_resize(mempool_t *pool, int new_min_nr, int gfp_mask);

销毁mempool：           
void mempool_destroy(mempool_t *pool);
```
mempool会分配一些内存块，空闲且不会得到使用，它浪费了大量内存。最好不要使用mempool而是处理可能的内存分配失败情况。



七、在栈上静态分配

1、在用户空间，有非常大的栈，栈空间还可以动态增长。相反，内核栈小而且固定。每个进程有2页的内核栈，每页大小为4KB（32位体系架构）或者8KB（64bit体系），所以通常它们的内核栈大小分别是8KB和16KB 。

2、单页内核栈：当激活这个功能选项配置时，每个进程的内核栈只有一页那么大，或者4KB，或为8KB。

3、为什么引入单页内核栈？

首先，可以让每个进程减少内存消耗。

其次，随着机器运行时间的增加，寻找2个未分配的、连续的页变得越来越难。物理内存渐渐变为碎片，因此给一个新进程分配虚拟内存的压力也在增大。

4、每个进程的整个调用链必须放在自己的内核栈中，中断处理程序也曾经使用它们所中断的进程的内核栈，这样，中断处理程序也要放在内核栈中。这样会把更严格的约束条件加在这内核栈上。当我们转而使用单页内核栈时，中断处理程序就不放在栈中了。

内核实现了一个新功能，中断栈。中断栈为每个进程提供了一个用于中断处理程序的栈。这样，中断处理程序不再和被中断的进程共享一个内核栈，它们可以使用自己的栈。对每个进程来说仅仅耗费了一页。

总的来说，内核栈可以是1页，也可以是2页，取决于具体的内核编译配置。当内核栈为2页时，中断处理程序和被中断进程共享内核栈。当内核栈为1页时，中断处理程序使用自己的中断栈。

内核中，在任意一个函数中，都必须尽量节省栈资源。只需要在具体的函数中让所有的局部变量所占空间之和不要超过几百字节。在栈上进行大量的静态分配（比如分配大型数组或大型结构体）很危险。要不然，在内核中和用户空间中进行的栈分配就没有什么差别了。栈溢出会产生严重的问题。因此，进行动态分配很适合内核。



八、per-CPU变量

1、支持SMP的操作系统支持使用每个CPU上的数据，每个CPU的数据存放在一个数组中， my_percpu[NR_CPUS] ， NR_CPUS 表示CPU的个数。使用这个数组时不需要锁，但是需要禁止内核抢占，内核抢占会引起2个问题：

如果代码被其他处理器抢占并且重新调度，那么此时的CPU变量就会无效，因为它指向的是错误的处理器。

如果另一个任务抢占了代码，那么有可能在同一个处理器上并发的访问 my_percpu ，产生竞态。

2、使用方法（Linux-2.4）：
```c
int cpu;
cpu = get_cpu();//获得当前处理器，并且禁止内核抢占
my_percpu[cpu]++;//或者其他操作
put_cpu();//激活内核抢占
```
3、新的CPU接口：

（1）编译时定义每个CPU的变量：
```c
DEFINE_PER_CPU(type, name);//为每个处理器都创建了一个类型为type，名字为name的CPU变量
DECLARE_PER_CPU(type,name);//在别处声明CPU变量
get_cpu_var(name)++;//增加该处理器上的name变量的值，get_cpu_var()返回当前处理器上的指定变量，同时禁止内核抢占。
put_cpu_var(name);//完成对CPU变量的操作，激活内核抢占
per_cpu(name,cpu)++;//增加指定cpu上的name变量的值，它不会禁止内核抢占，页不会提供锁保护，必须自己先进行锁保护，再使用这个函数。
```
（2）运行时的每个cpu变量：
```c
动态分配每个cpu变量
void *alloc_percpu(type);
//封装了__alloc_percpu，它给系统的每个cpu分配一个指定类型，按照给定type的自然边界对齐

void *__alloc_percpu(size_t size, size_t align);
//size为要分配的字节数，align为分配时按多少字节对齐

void free_percpu(const void *);
//释放每个cpu上的数据

使用方法：

void *percpu_ptr;
unsigned long *foo;
percpu_ptr = alloc_percpu(unsigned long);

foo = get_cpu_var(percpu_ptr);
//返回一个void *指针，它指向处理器的数据，同时禁止内核抢占

//操作foo

put_cpu_var(percpu_ptr);
//完成，激活内核抢占。
```
4、为什么要使用每个cpu的数据？

减少了数据锁定，不会产生竞态，但是它需要禁止抢占。

它可以大大减少缓存失效。如果一个处理器操作某个数据，而该数据存放在其他处理器的缓存中，那么该处理器必须清理或刷新自己的缓存。持续不断的缓存失效称为缓存抖动，影响系统性能。使用每个cpu数据使缓存影响减到最低，因为每个cpu只会访问自己的数据，不会访问其他cpu的数据。

注意，不要在访问每个cpu的数据时候睡眠，否则，当醒来后已经到了其他处理器上了。

per-CPU主要用于网络子系统，它的数据比较多，使用per-CPU处理速度快，不需要锁。



九、在系统引导时获取大的缓冲区

大的、连续的内存缓冲区分配容易失败，系统的内存会随着时间的推移而碎片化，导致无法获取真正的大内存区域。可以在系统引导时获取专用缓冲区，在引导时就分配内存是获得大量连续内存的唯一方法。
```c
void *alloc_bootmem(unsigned long size);
void *alloc_bootmem_low(unsigned long size);
void *alloc_bootmem_pages(unsigned long size);
void *alloc_bootmem_low_pages(unsigned long size);
void free_bootmem(unsigned long addr, unsigned long size);
```


十、直接内存访问DMA

1、DMA是一种硬件机制，它允许外围设备和内存之间直接传输它们的I/O数据，而不需要cpu的参与。

2、在引导时，可以通过向内核传递“ mem=参数 ”的办法保留顶部RAM，比如系统有256MB内存，参数 mem=255M 将使内核不能使用顶部的1M字节。随后，模块使用下面的代码获得对该内存的访问权：
```c
dmabuf = ioremap(0xFF00000/*255M*/,  0x100000/*1M*/);
```
3、根据DMA缓冲区期望保留时间的长短，分为2种DMA映射：

（1）一致性DMA映射：这种类型的映射存在于驱动程序生命周期中，它的生命周期很长。一致性映射的缓冲区必须同时被cpu和外围设备访问。
```c
void *dma_alloc_coherent(struct device *dev, size_t size,  dma_addr_t *dma_handle, int flag);
返回值是缓冲区的内核虚拟地址，可以被驱动程序使用。而与它相关的总线地址，保存在 dma_handle 中，
这个缓冲区可以给IIC的i2c_master_send()使用。

void dma_free_coherent(struct device *dev,  size_t size, void *vaddr, dma_addr_t dma_handle);
```
（2）流式DMA映射：通常为单独的操作建立流式映射。流式映射可以被优化。流式映射比一致性映射具有更为复杂的接口。
```c
dma_addr_t dma_map_single(struct device *dev, void *buffer, size_t size, enum dma_data_direction direction);
返回值是总线地址，可以把它传递给设备使用。

删除映射：
void dma_unmap_single(struct device *dev, dma_addr_t dma_addr, size_t size, enum dma_data_direction direction);
```
一旦流式缓存区被映射，它只属于设备，而不是处理器。知道缓冲区被撤销映射前，驱动程序不能访问缓冲区的内容。



参考内容：
LKD 、 LDD

