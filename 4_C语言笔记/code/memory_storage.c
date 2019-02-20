/***************************************************************************
输出结果：
c=0x63c010,a=0x7ffc4d9fadf4,b=0x601058,e=0x601054,d=0x60105c,f=0x400658
***************************************************************************/
#include<stdio.h>
#include<stdlib.h>

int d;
static int e;
char * f="abcdefg";

int main(void)
{
	int a;
	static int b;
	int * c=(int *)malloc(sizeof(int));

	printf("c=%p,a=%p,b=%p,e=%p,d=%p,f=%p\n",c,&a,&b,&e,&d,f);
	return 0;
}
/*
a=0x7ffec6d3f544，栈区
c=0x14ff010，堆区
d=0x60105c，静态区
b=0x601058，静态区
e=0x601054，静态区
f=0x400658，常量区
*/
