/***************************************************************************
输出结果：
a==b
a==d
b==d
a==c
b==c
d==c
***************************************************************************/
#include<stdio.h>

#define d 1
int main(void)
{
	int a=1;
	unsigned b=0x01;
	char c=1;
	if(a==b)
		printf("a==b\n");
	if(a==d)
		printf("a==d\n");
	if(b==d)
		printf("b==d\n");
	if(a==c)
		printf("a==c\n");
	if(b==c)
		printf("b==c\n");
	if(d==c)
		printf("d==c\n");

	return 0;
}
