/***************************************************************************
输出结果：
call func
call func1
call func2
***************************************************************************/
#include<stdio.h>

void func(void)
{
	printf("call func\n");
}

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
	void(*p)();
	p=func;
	(*p)();

	void(*q[2])();
	q[0]=func1;
	q[1]=&func2;
	q[0]();
	q[1]();

	return 0;
}
