
```c
1. ioctl 属于内核线程还是用户线程？
看摄像头的kernel log中有：
<4>[  155.789506] .(2)[3185:AFthread]moveAF, 204.  move [curr] 8 [target] 108

在用户空间hal层有代码：
::prctl(PR_SET_NAME,"AFthread", 0, 0, 0);
它用于重命名线程的名字为 AFthread 。

moveAF, 204.  move [curr] 8 [target] 108，这个log位于kernel的ioctl函数moveAF()中。

所以，ioctl系统调用属于用户空间的线程。

2. 用户空间在执行ioctl系统调用时，会让处理器进入内核态，但是这个线程仍然属于用户空间。

3. 为什么需要系统调用？
Linux系统分为用户态和内核态，优先级别分别为1和0，内核态具有最高的权限。
用户态的资源访问权限受到限制，内核态的资源访问权限不受限制。
处于安全的考虑，某些操作只能在内核空间中进行，比如对硬件相关的操作、异常和中断处理。
在用户态向内核态的切换过程中，由用户栈转为内核栈，保存现场可能会带来性能损耗。

4. 用户态切换到内核态的方式：
1）系统调用：如read/write/malloc。
2）异常：当CPU正在运行用户空间的程序时，发生了一个异常，会导致当前程序切换到处理异常的内核程序中，进入内核态，比如用户空间访问空指针。
3）外围设备的中断：
CPU正在执行用户程序，此时它收到了一个中断信号，CPU就会切换到内核态去执行中断处理程序。

5. 使用 ioctl 时，常用的 IOR 和 IRW 
 _IO(type,nr) //没有参数的命令
_IOR(type,nr,size) //该命令是从驱动读取数据
_IOW(type,nr,size) //该命令是从驱动写入数据
_IOWR(type,nr,size) //双向数据传输


一、 以 mtkcam LENS HAL层代码为例

#define mcuIOC_G_MOTORINFO    				_IOR('A',0,mcuMotorInfo)
#define mcuIOC_T_MOVETO       						_IOW('A',1,u32)
#define mcuIOC_T_SETINFPOS    					_IOW('A',2,u32)
#define mcuIOC_T_SETMACROPOS  				_IOW('A',3,u32)
#define mcuIOC_G_MOTORCALPOS  			_IOR('A',4,mcuMotorCalPos)
#define mcuIOC_S_SETPARA      						_IOW('A',5,mcuMotorCmd)
#define mcuIOC_S_SETDRVNAME   				_IOW('A',10,mcuMotorName)
#define mcuIOC_G_MOTOROISINFO 			_IOR('A',12,mcuMotorOISInfo)




#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),(_IOC_TYPECHECK(size)))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))


# define _IOC_WRITE	1U
# define _IOC_READ	2U

#define _IOC_TYPECHECK(t) (sizeof(t))

#define _IOC(dir,type,nr,size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))


#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)       //8
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS) //16
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)   //30



#define _IOC(dir,type,nr,size) \
	(((dir)  << 30) | \
	 ((type) << 8) | \
	 ((nr)   << 0) | \
	 ((size) << 16))

读/写		bit31~bit30 	2位为 “区别读写” 区，作用是区分是读取命令还是写入命令。
size			bit29~bit16 	14位为 "数据大小" 区，表示 ioctl() 中的 arg 变量传送的内存大小。
type			bit15~bit08  	8位为 “魔数"(也称为"幻数")区，这个值用以与其它设备驱动程序的 ioctl 命令进行区别。
nr			bit07~bit00    8位为 "区别序号" 区，是区分命令的命令顺序序号。




ioctl 传递参数：

1、参数为整型，kernel中可以直接使用这个数据；
2、参数为指针，kernel中必须使用 cpoy_from_user()  和  cpoy_to_user()

3、传递整型时，一般使用 _IO 
 _IO(type,nr) //没有参数的命令
_IOR(type,nr,size) //该命令是从驱动读取数据
_IOW(type,nr,size) //该命令是从驱动写入数据
_IOWR(type,nr,size) //双向数据传输


4、 因为只有传递指针时，才会使用 _IOR 和 _IOW 和 _IOWR ，这样，在 kernel 中会使用 cpoy_from_user 和 cpoy_to_user 来检查参数的安全性，不会恶意传参。


5、 cpoy_from_user 和 cpoy_to_user 的作用：用来检查用户空间传给内核空间的指针是否安全。

6、一般内核空间都可以访问用户空间的地址。

```


