#include<stdio.h>

//判断x是否为2的整数次幂
int main(void)
{
	int x=8,a;
	a=((x&(x-1))==0) && (x!=0);

	printf("a=%d\n",a);
}

