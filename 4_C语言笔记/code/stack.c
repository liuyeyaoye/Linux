/***************************************************************************
输出结果：
hello world
***************************************************************************/
#include<stdio.h>

char * getMemory(void)
{
	char p[]="hello world";
	return p;
}

int main(void)
{
	char *str=NULL;
	str=getMemory();
	printf("%c%c%c%c%c%c%c%c%c%c%c\n",*str,*(str+1),*(str+2),*(str+3),*(str+4),*(str+5),*(str+6),*(str+7),*(str+8),*(str+9),*(str+10));
}
