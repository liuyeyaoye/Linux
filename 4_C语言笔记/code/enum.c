/***************************************************************************
输出结果：
4
***************************************************************************/
#include<stdio.h>

enum TEST{
	zero=0,
	one,
	two,
	three,
	four
}NUM;

int main(void)
{
	unsigned int a = sizeof(NUM);
	printf("%d\n",a);	
	return 0;
}
