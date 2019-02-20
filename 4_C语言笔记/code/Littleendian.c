/***************************************************************************
输出结果：
1(1:little.0:big)
***************************************************************************/
#include<stdio.h>

int isLittleendian(void)
{
	union A{
		char i;
		int j;
	}a;
	a.j=1;
	return (a.i ==1);
}

int main(void)
{
	int a=isLittleendian();
	printf("%d(1:little.0:big)\n",a);
	return 0;
}
