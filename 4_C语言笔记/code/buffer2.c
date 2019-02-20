/***************************************************************************
输出结果：
*p=a
***************************************************************************/
#include<stdio.h>

char * test(void)
{
	char a[5]="abcd";
	char b[5];
	strcpy(b,a);
	return b;
}

int main(void)
{
	char *p;
	p=test();
	printf("*p=%c\n",*p);
	return 0;
}
