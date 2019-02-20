/***************************************************************************
输出结果：
before sizeof, a=49, b=2
sizeof(a=b+1)=1
after sizeof,a=49,b=2
***************************************************************************/
#include<stdio.h>

int main(void)
{
	char a='1';
	int b=2;
	int c;
	printf("before sizeof, a=%d, b=%d\n",a,b);
	c=sizeof(a=b+1);
	printf("sizeof(a=b+1)=%d\n",c);
	printf("after sizeof,a=%d,b=%d\n",a,b);

	return 0;
}
