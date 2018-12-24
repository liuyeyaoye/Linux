


```c
1.  i++ 和 ++i 的效率：
前缀 ++ 可以返回对象的引用，后缀 ++ 必须返回对象的值。所以：
内建数据类型的情况，二者的效率没有区别。
自定义数据类型的情况， ++i效率较高。



2、 有符号和无符号的转换

例子：
#include<stdio.h>
int main(void)
{
	int i=7, j=-7, k=-8;
	int m,n;
	unsigned int a=i;
	m=(a+j==0)?1:0;
	n=(a+k>0)?1:0;
	printf("m=%d,n=%d\n",m,n);
	return 0;
}

输出：
m=1,n=1

分析：
a的类型为unsigned，j的类型为signed，a与j相加时，j会隐式转换成unsigned。
因为int类型为32位，位数太多，可以将它看做8位的char类型来计算：
j=-7的补码为：
原码：1000 0111
反码：1111 1000
补码：1111 1001
j隐式转换成unsigned：
1111 1001
a的补码：
0000 0111
a+j的补码：
1 0000 0000
溢出一位，结果为0。
故unsigned的7加上signed的-7的结果为0.

k=-8的补码为：
原码：1000 1000
反码：1111 0111
补码：1111 1000
a的补码：
0000 0111
a+k的补码：
1111 1111
结果大于0.



3、不使用临时变量，交换2个值
void swap1(int& a, int& b)
{
	a=a+b;
	b=a-b;
	a=a-b;
}

void swap2(int& a, int& b)
{
	a ^= b;
	b ^= a;
	a ^= b;
}



4. 数值溢出
char类型的范围：-128~127。127+1=-128。-128-1=127。
unsigned char类型的范围：0~255。255+1=0。0-1=255.
溢出总结：
最大值加1，等于最小值。
最小值-1，等于最大值。
-128的补码为 1000 0000,它没有原码和反码。



5、 关于 sizeof

例子：
#include<stdio.h>
void func(char str[])
{
	printf("in func, sizeof(str)=%d\n",sizeof(str));
}
int main(void)
{
	char str[] = "abcde";
	printf("sizeof(str)=%d\n",sizeof(str));
	func(str);
	return 0;
}

输出：
sizeof(str)=6
in func, sizeof(str)=8



6.
int(*a[10])(int);
a是一个有10个指针元素的数组，每个指针指向一个参数为int、返回值为int的函数。

int(*(*a)[5])(int *p);
a是一个指针，指向有5个元素的数组，数组的每个元素是函数指针，函数指针指向返回值为int、参数为int*的函数。

int(*(*a)(int *p))[5];
a是一个函数指针，函数的形参为int *，返回值是指向数组的指针，该数组具有5个int类型的元素。


void(*signal(int signo, void(*func)(int)))(int);
==> 
typedef void(*signal_t)(int);
signal_t signal(int signal, signal_t func);


char (*(*x())[5])();
x是一个无参数函数的声明，该函数返回一个指针，这个指针指向一个有5个指针元素的数组，该数组的每个元素都指向一个返回值类型是char的无参函数。


复杂类型声明时，先看它的右边是什么符号，再看它的左边是什么符号。
如果是a[]，则它是一个数组。
如果是a()，则它是一个函数。
如果是*a，则它是一个指针。



7、字符串的存储

例子：
#include<stdio.h>
int main(void)
{
	char * str1 = "abc";
	printf("%p\n",str1);
	printf("%p\n","abc");
	printf("%p\n","abcd");
	return 0;
}

输出：
0x400614
0x400614
0x40061c

字符串 "abc" 与字符串 "abc" 的地址相同。
字符串 "abc" 与字符串 "abcd" 的地址不同。



8、 指针运算
p+n=p+n*sizeof(*p)
p2-p1=(p2-p1)/sizeof(*p)

例子：
#include<stdio.h>
int main(void)
{
	char * str[]={"welcome","to","fortemedia","nanjing"};
	char **p=str+1;
	str[0]=(*p++)+2;
	str[1]=*(p+1);
	str[2]=p[1]+3;
	str[3]=p[0]+(str[2]-str[1]);
	printf("str[0]=%s\n",str[0]);
	printf("str[1]=%s\n",str[1]);
	printf("str[2]=%s\n",str[2]);
	printf("str[3]=%s\n",str[3]);
	return 0;
}

输出：
str[0]=
str[1]=nanjing
str[2]=jing
str[3]=g

分析：
字符串表达式 "welcome" 等价于它的首地址。数组str保存的是这些字符串的首地址。



9、调用地址为0处的程序：
(*((void(*)())0))();  等价于：
typedef void(*func)();
(*((func)0))();



10. 函数指针
void(*sigal(int, void(*)(int)))(int);  等价于：
typedef void(*hander)(int);
hander sigal(int, hander);
```

