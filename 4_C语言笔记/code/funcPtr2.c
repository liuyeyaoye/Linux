/***************************************************************************
输出结果：
call func1
call func2
***************************************************************************/
#include<stdio.h>

void func1(void)
{
	printf("call func1\n");
}

void func2(void)
{
	printf("call func2\n");
}

int main(void)
{
	void(*a[2])();
	void(*(*pf)[2])();
	pf=&a;

	a[0]=func1;
	a[1]=func2;

	pf[0][0]();
	pf[0][1]();

	return 0;
}
