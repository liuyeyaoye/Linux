


一、 container_of 的实现：

```c
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

1、ptr是一个指向结构体成员member的指针， type 是结构体的类型，member是结构体中的某个成员的名称。
这个宏返回的是整个结构体的首地址。如果知道某个结构体中元素的指针，可以根据它来反推整个结构体的首地址。

2、GCC对标准C的扩展语法 ({}) ：  x = ({1;2;}); x的值为2。

3、 
const typeof( ((type *)0)->member ) *__mptr = (ptr);
//获得结构体成员 member 的类型，然后用它来定义一个临时指针，将ptr的值赋给这个临时指针。这样做的目的是避免对传入的ptr直接进行操作。

(type *)( (char *)__mptr - offsetof(type,member) );
//用__mptr减去member的偏移量，即为这个结构体的首地址，类型为 type *。

4、对这个宏的简单化处理：
 #define container_of(ptr, typex, member) (typex *) ( (size_t)ptr- offsetof(typex, member) ) 
```

二、 offsetof 的实现

```c
#define offsetof(TYPE, MEMBER)	((size_t)&((TYPE *)0)->MEMBER)

TYPE是结构体的类型，MEMBER是结构体中的一个成员，offset宏返回的是MEMBER在TYPE结构体中相对于首地址的偏移量。

(TYPE *)0 ，将0强制转换成一个指针，这个指针指向的类型为 TYPE 。
虽然0是一个非法指针，然而此处并没有真正对其进行内存访问（运行期），只是利用其进行计算（编译期），所以不会造成任何程序错误。

((TYPE *)0)->MEMBER ,通过指针来访问结构体中成员member。
&((TYPE *)0)->MEMBER ，得到member成员的地址，因为结构体的首地址为0，所以member成员的地址就是member成员相对于结构体首地址的偏移量。
```

