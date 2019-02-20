/***************************************************************************
输出结果：
n=4, str=4096
***************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(void)
{
	int m=4096;
	int n;
	char str[10]={0};
	n=sprintf(str, "%d", m);
	printf("n=%d, str=%s\n",n,str);
}
