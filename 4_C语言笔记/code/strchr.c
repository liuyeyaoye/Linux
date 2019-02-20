/***************************************************************************
输出结果：
p=0x0,q=0x0
***************************************************************************/
#include<stdio.h>
#include<string.h>
#include <assert.h>

char *strchr1(const char *str, char c)
{
	assert(str != NULL);
	char * ptr = str;

	while(*ptr != '\0'){
		if(*ptr == c)	break;
		++ptr;
	}
	if(c == '\0')	return ptr;
	return (*ptr == '\0') ? NULL : ptr;
}

int main(void)
{
	char * str="abcdefg";
	char *p, *q;
	p=strchr(str,0);
	q=strchr1(str,0);
	printf("p=0x%x,q=0x%x\n",*p,*q);
	return 0;
}
