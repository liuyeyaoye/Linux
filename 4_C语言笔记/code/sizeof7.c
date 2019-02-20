/***************************************************************************
输出结果：
sizeof(char)=1,sizeof(short int)=2
a+b=0
sizeof(a+b)=4
***************************************************************************/
#include<stdio.h>

int main(void)
{
	char a=0;
	short int b=0;
	printf("sizeof(char)=%d,sizeof(short int)=%d\n",sizeof(char),sizeof(short int));
	printf("a+b=%d\n",a+b);
	printf("sizeof(a+b)=%d\n",sizeof(a+b));

	return 0;
}
