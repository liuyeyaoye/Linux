//不使用>,<,if，比较正整数a,b的大小。

#define max(a,b) (((a)-(b))&0x80000000)?(b):(a)

#include<stdio.h>

int main(void)
{
	int i;
	i=max(1,9);
	printf("i=%d\n",i);

	i=max(5,1);
	printf("i=%d\n",i);

	i=max(1,1);
	printf("i=%d\n",i);

	return 0;
}



