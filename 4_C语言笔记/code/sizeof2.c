/***************************************************************************
输出结果：
inMain=100
inFunc=8
***************************************************************************/
#include<stdio.h>

void func(char str[100])
{
	int inFunc = sizeof(str);
	printf("inFunc=%d\n",inFunc);
}

int main(void)
{
	char str[100];
	int inMain = sizeof(str);
	printf("inMain=%d\n",inMain);
	func(str);
	return 0;
}
