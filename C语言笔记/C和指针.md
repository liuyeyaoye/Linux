
```c
1.关于sizeof

#include<stdio.h>
int main(void)
{
	char a=1;
	int b=2;
	int c;
	printf("before sizeof, a=%d, b=%d\n",a,b);
	c=sizeof(a=b+1);
	printf("sizeof(a=b+1)=%d\n",c);
	printf("after sizeof,a=%d,b=%d\n",a,b);
	return 0;
}

输出：
before sizeof, a=1, b=2
sizeof(a=b+1)=1
after sizeof,a=1,b=2
分析：
sizeof(a=b+1)  等价于
sizeof(a)



2.关于ASCII码
char a=1；//a的ASCII码为1，输出1.
char a='1'; //a的ASCII码为49，输出49.
分析：可以直接对char类型赋值一个数字，不必加''，此时这个数字就表示ASCII码。



3、隐式类型转换遵守整型提升的原则，如char转换成int，int转换成unsigned int。



4、堆、栈、静态区、常量区、RAM、ROM的区别
（1）堆：由程序员动态分配和释放。使用malloc()或new()分配，使用free()或delete释放。
（2）栈：由编译器自动分配和释放，存放函数的参数值和函数的局部变量。用户空间可用的栈为8M。
（3）静态区(全局区)：存放全局变量和static变量。
（4）常量区：存放常量，如字符串"abcdefg"、数字8等都是常量。
（5）RAM：有DRAM/DDRAM(DDR)，掉电数据丢失。读取数据的速度快。平时说的内存都是指RAM（NVRAM掉电数据不会丢失）。
（6）ROM：掉电数据不会丢失。读取数据的速度慢。手机中的ROM可读可写，用来存储程序。
（7）内存：内存存放当前正在执行的程序和数据。内存指的是RAM。在程序运行时，CPU从ROM中读取程序到RAM中去运行。
（8）堆、栈、静态区、常量区都在内存（RAM）中。



5、常见的操作符优先级
()：表达式符号、函数调用。左结合性。
[]：数组下标引用。左结合性。
. ：访问结构体成员。左结合性。
->：访问结构体指针成员。左结合性。
++：后缀自增。左结合性。
--：后缀自减。左结合性。
！：逻辑反。右结合性。
~ ：按位取反。右结合性。
+ ：单目，表示正值。右结合性。
- ：单目，表示负值。右结合性。
++：前缀自增。右结合性。
--：前缀自减。右结合性。
* ：间接访问指针。右结合性。
& ：取地址。右结合性。
sizeof：取长度，以字节表示。右结合性。
(类型)：类型转换。右结合性。
算术运算符：*，/，%，+，-，<<，>>。左结合性。
关系运算符：>，>=，<，<=，==，!=。左结合性。
位运算符：&，^，|。左结合性。
逻辑运算符：&&，||。左结合性。
条件运算符:?:
赋值运算符：=，+=，-=，*=，/=，%=，<<=，>>=，&=，^=，|=。右结合性。
逗号运算符：,。左结合性。



6、 0，'\0'，'0'的区别：

#include<stdio.h>
int main(void)
{
	char a=0;
	char b='\0';
	char c='0';
	char d=a;
	char e;

	printf("a=0x%x,b=0x%x,c=0x%x,d=0x%x,e=0x%x\n",a,b,c,d,e);
	return 0;
}
输出：
a=0x0,b=0x0,c=0x30,d=0x0,e=0x0



7、实现 strchr 函数
#include<stdio.h>
#include<string.h>
#include <assert.h>

char *strchr1(const char *str, char c)
{
	assert(str != NULL);
	char * ptr = str;

	while(*ptr != '\0'){
		if(*ptr == c)	break;
		++ptr;
	}
	if(c == '\0')	return ptr;
	return (*ptr == '\0') ? NULL : ptr;
}

int main(void)
{
	char * str="abcdefg";
	char *p, *q;
	p=strchr(str,0);
	q=strchr1(str,0);
	printf("p=0x%x,q=0x%x\n",*p,*q);
	return 0;
}



8、指针运算
指针只有2种运算
（1）指针 +/- 整数
ptr+n：等于 ptr+sizeof(*ptr)*n。

（2）指针 - 指针
2个指针必须指向同一类型：
ptr1-ptr2：等于 (ptr1-ptr2)/sizeof(*ptr1)

例子：
#include<stdio.h>
int main(void)
{
	unsigned int a[6]={1,2,3,4,5,6};
	unsigned int *ptr1,*ptr2;
	ptr1=&a[1];
	ptr2=&a[4];
	printf("ptr1=%ld,ptr2=%ld,ptr1+2=%ld,ptr2-ptr1=%d,ptr1-ptr2=%d\n",ptr1,ptr2,ptr1+2,ptr2-ptr1,ptr1-ptr2);

}

输出：
ptr1=140723292297620,ptr2=140723292297632,ptr1+2=140723292297628,ptr2-ptr1=3,ptr1-ptr2=-3



9. 数组名a和&a的区别

#include<stdio.h>
int main(void)
{
	int a[5]={1,2,3,4,5};

	printf("a=%ld,a+1=%ld,&a=%ld,&a+1=%ld\n",a,a+1,&a,&a+1);
	return 0;
}

输出：
a=140727773443920,a+1=140727773443924,&a=140727773443920,&a+1=140727773443940
数组名a是指向数组首元素的指针，数组名等价于&a[0]。
&a是指向整个数组的指针。
但是sizeof(a)表示的却是整个数组的长度。
不能使用a++这样的表达式，因为a是一个常量，它的值不能改变。



10、 数组和字符串常量
char message1[] = "hello";
char * message2 = "hello";

二者的存储方式不同。
数组 message1 是变量， 存储的成员为： 'h','e','l','l','o','\0'.
指针 message2 指向字符串常量"hello"的首地址。不能修改 message2 指向的内容，不能对 message2 使用 memset()和snprintf()。



11、二维数组
int m[3][10];
m是数组名，它是指向二维数组第一个元素的指针，它是指向一个包含10个整型元素数组的指针。
int (*p)[10]=m;  p指向m的第一行。



12、 extern 关键词
extern 可以声明变量和函数。
它修饰变量时，表示声明一个变量，不能省略。不加extern就是定义一个变量，而不是声明。
它声明一个函数时，可以省略。编译器默认会加上。
在定义函数时，不需要加extern。函数默认都是外部函数，可以被其他文件引用。如果加了 static 才是内部函数。



13、 MT6260平台中， 2个不同的 .c 文件不能使用 #ifdef
在一个文件中使用： #define WiFI_ESP8266
在另外一个文件中不能： #ifdef WiFI_ESP8266
可以在makefile 中统一定义宏。



13、 a++ 等价于 a=a+1; 等价于赋值语句。 所以对数组名不能使用 a++ 。



14. 字符串常量
"xyz";
这个表达式的值是一个指针，指向字符串常量第一个字符。
"xyz"+1;
字符串常量加1，这个表达式的值是一个指针，指向字符 y 。



15、
插入排序：
void insert(int *a, int n)
{
	int i,j,temp;
	for(i=1;i<n;i++){
		temp=a[i];
		for(j=i; j>0 && a[j-1]>temp; j--)
			a[j]=a[j-1];
		a[j]=temp;
	}
}
```


