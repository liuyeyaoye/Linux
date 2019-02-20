/***************************************************************************
输出结果：
str=0x2558010
str=0x2558010
***************************************************************************/
#include<stdio.h>
#include<string.h>

int main(void)
{
	char *str=(char *)malloc(100);
	strcpy(str,"hello");
	printf("str=0x%x\n",str);
	free(str);
	printf("str=0x%x\n",str);
	return 0;
}
