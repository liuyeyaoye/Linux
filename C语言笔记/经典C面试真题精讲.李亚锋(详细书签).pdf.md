

```c

没有完成的：
P31：编程求最大值的阶乘
P33:字符旋转
P67：栈迷宫问题
P73:队列问题
P80：约瑟夫问题
P82:二叉树遍历
P83:modbus CRC
P85:汉诺塔
P88:找符合条件的数
P91：hash查找
P205：判断链表是否存在环

1、 这个表达式的含义：


char ( *( *x() )[5] )();

x是一个无参数函数的声明，该函数返回一个指针。
返回的这个指针指向一个有5个指针元素的数组。
该数组的每个元素都可指向一个返回值类型是char的无参函数。
等价于：
typedef char (*Func)();	==> Func (*x())[5];
typedef Func A[5]; 		==> A *x();


2. 一个 ++ 的例子


#include<stdio.h>
int main(void)
{
	int i=1;
	int j=4;
	while(i++ > j++)
		;
	printf("i=%d,j=%d\n",i,j);
	return 0;
}
输出：
i=2,j=5


3.


char 类型的 127加1，结果是 -128.
最大的char+1，结果为最小的char。
最大的int+1，结果为最小的int。
char的范围是： -128—127
127:  0111 1111
1:    0000 0001
0:	  0000 0000
-1:	  1111 1111
-127: 1000 0001
-128: 1000 0000(-128没有原码和反码)
char为8位，一共可以表示256个数字，-128—127正好是256个数字。


4.


在gcc中：
format ‘%d’ expects argument of type ‘int’
format ‘%x’ expects argument of type ‘unsigned int’
format ‘%p’ expects argument of type ‘void *’

对于 %x，当一个数字的二进制的最高位为1时，它会当做一个负数，强制转换成一个unsigned int数。
char类型输入为 0xE5 ， %x会输出： ffffffe5
char类型输入为 0x39 ， %x正常输出： 39

（1）char类型的0xE5 是怎么转换成 unsigned int类型的ffffffe5 的？
在前面加1111，加到unsigned int的32位即可。

（2）int类型怎么转换成unsigned int类型：
int类型输入为： 0xB9FFE01F ， %x会输出： B9FFE01F
int类型输入为： 0x3FFC2AAE ,  %x会输出： 3FFC2AAE


5.


%10.5s	:每行固定占10个字符，右对齐显示从头开始的5个字符。
%-10.5s :每行固定占10个字符，左对齐显示从头开始的5个字符。
例子：
char *str="Hello,Shenzhen";
printf("%10.5s\n",str);
printf("%-10.5s\n",str);
输出：
     Hello
Hello    


6. strcpy 实现：考虑内存重叠(test50.c)


char * mystrcpy(char * dest, char * src)
{
	int n=strlen(src)+1;
	char *p=dest;

	if(dest==NULL || src==NULL || *dest=='\0' || *src=='\0'){
		return NULL;
	}

	if(dest==src){
		return src;
	}

	if( (dest>src) && (dest<(src+n)) ){
		dest = dest+n-1;
		src = src+n-1;
		while(n--){
			*dest-- = *src--;
		}
	}
	else{
		while(n--){
			*dest++ = *src++;
		}
	}
	
	return p;
}


7. 多维数组


a[1]=*(a+1);
a[0][2]=*(a[0]+2);
a[0][0][1]=*(a[0][0]+1);
a[0][2][0]=*(a[0][2]+0)=*a[0][2]=**(a[0]+2);

(1)二维数组
二维数组名为a：
a指向的内存占8个字节；
&a[0]指向的内存占8个字节；
a[0]指向的内存占4个字节；

#include<stdio.h>
int main(void)
{
	int a[2][2]={1,2,3,4};
	printf("a=%x, a+1=%x\n",a,a+1);
	printf("&a[0]=%x, &a[0]+1=%x\n",&a[0],&a[0]+1);
	printf("a[0]=%x,a[0]+1=%x\n",a[0],a[0]+1);
	return 0;
}
输出：
a=f882ce70, a+1=f882ce78
&a[0]=f882ce70, &a[0]+1=f882ce78
a[0]=f882ce70,a[0]+1=f882ce74


8. 函数指针


void(*signal(int signo, void(*func)(int) ) ) (int);
==> 
typedef void(*signal_t)(int);	
signal_t signal(int signal, signal_t func);


9.


__attribute__((__packed__)) :让编译器取消对代码的优化对齐，按实际占用字节数进行对齐。

函数的执行顺序：
printf("%d,%d",f1(),f2());
函数的执行顺序为： printf,f2(),f1()

1、 网络字节序是大端模式。

2、几个函数的区别
(1)write/read：读写函数。
(2)send/recv：多了一个flag参数，可以指定消息类型。
(3)sendto/recvfrom：多了一个addr地址参数，可以指定具体的地址进行发送或接收。所以它既可以用于TCP，也可以用于UDP。
(4)sendmsg/recvmsg：可以发送/接收辅助数据 cmsg 。

3、数值溢出运算
char chr=127;
int sum=100;
chr += 1;
sum += chr;
计算后，sum=-28;
如果没有写 unsigned ，默认为 signed 。 chr 加1得到 -128.
sum= 100 + (-128) =28.

4、 正数和负数相加的本质
1+(-4) = -3.
1的补码：  0000 0001
-4的补码： 1111 1100
结果：	   1111 1101
转换为原码： 1000 0011 即 -3。

100 + (-128)
100的补码：  0110 0100
-128的补码： 1000 0000
结果：  	 1110 0100	
转换为原码： 1001 1100 即 -28.

5. 指针偏移运算
struct data{
	long i;
	char * j;
	short k;
	char l;
	short m[5];
}*p;
p=(struct data*)0x10000000;
printf("%p\n",p+sizeof(*p));
printf("%p\n",p+1);
printf("%p\n",(unsigned long)p+1);
printf("%p\n",(int *)p+1);
printf("%p\n",(char *)p+1);

依次输出p值为：
0x10000240
0x10000018
0x10000001
0x10000004
0x10000001

6、指向绝对地址执行
让程序从内存地址为 0x20000000 执行：
*((void (*)())0x20000000)();
等价于：
typedef void(*func)();
(*((func)0x20000000))();
或者：((func)0x20000000)();


10.


1. 宏定义
(1)在 "" 中不要使用宏，否则会被当做一个字符串使用
(2)宏定义里的 # 用于把参数变成字符串， ## 用于把2个参数连接起来。
(3)当一个宏的参数是另外一个宏时，宏定义里面有"#","##"的宏参数不会再展开（没有 # 和 ## 的宏参数会再展开），此时要使用转换宏：
#define _STR(x) #x
#define STR(x) _STR(x)
使用 STR 宏可以把 x 变成字符串 "x"

2. static 修饰变量与函数的作用
修饰变量：
静态变量只初始化一次，如果没有初始化则自动初始化为0，程序运行结束静态变量才被释放。
静态变量的作用范围只在本文件，其他文件不能使用它。
修饰函数；
静态函数的作用范围只在本文件，其他文件不能使用它。

3.内存地址：
(1)分为数据区和代码区，存储数据和代码。
栈区>堆区>静态区>常量区>代码区
(2)栈内地址不能传递到函数之外，比如栈指针、栈数组名。


11. 二分查找


(1)二分查找用在对已经排序的数据中查找，适合在大量的数据中查找，如文件的查找、联系人的查找。
(2)实现：
int search(int n, int a[], int len)
{
	int mid=0, min=0, max=len-1, result=0;
	while(min<=max){
		mid=(min+max)/2;
		if(n==a[mid]){
			result=1;
			break;
		}
		else if(n>a[mid]){
			min=mid+1;
		}
		else{
			max=mid-1;
		}		
	}
	return result;
}
n为要查找的数，a为有序数组，len为数组长度。如果找到返回1，找不到返回0.


12. 排序算法


1. 选择排序
(1)原理：
首先将下标为0的数跟其他的n-1个数比较，将最大或最小的数放在下标为0处；
然后将下标为1的数跟其他的n-2个数比较，将次大或次小的数放在下标为1处；
以此类推，直到排序完成。

(2)代码实现：
void search(int a[], int len)
{
	int i=0,j=0,temp=0;
	for(i=0; i<len; i++){
		for(j=i+1;j<len;j++){
			if(a[i]>a[j]){
				temp=a[i];
				a[i]=a[j];
				a[j]=temp;
			}
		}
	}
}
a为数组名，len为数组长度。

2、冒泡排序
(1)原理：
将相邻的数进行大小比较。

(2)代码实现：
void search(int a[], int len)
{
	int i=0,j=0,temp=0;
	for(i=0; i<len; i++){
		for(j=1;j<len-i;j++){
			if(a[j]<a[j-1]){
				temp=a[j];
				a[j]=a[j-1];
				a[j-1]=temp;
			}
		}
	}
}

3、直接插入排序
(1)原理：
将n个数据分为有序和无序部分，每次将无序数列的第一个元素跟有序数列从后往前逐个进行比较，找到合适的插入位置。

(2)代码实现：
void search(int a[], int len)
{
	int i=0,j=0,min=0;
	for(i=0;i<len;i++){
		min=a[i];
		for(j=i-1; (j>=0)&&(a[j]>min); j--)
			a[j+1]=a[j];
		a[j+1]=min;
	}
}

```


