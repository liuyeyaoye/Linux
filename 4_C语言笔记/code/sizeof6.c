/***************************************************************************
输出结果：
sizeof(str)=6
in func, sizeof(str)=8
***************************************************************************/
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
