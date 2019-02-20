/***************************************************************************
输出结果：
str[0]=
str[1]=nanjing
str[2]=jing
str[3]=g
***************************************************************************/
#include<stdio.h>

int main(void)
{
	char * str[]={"welcome","to","fortemedia","nanjing"};
	char **p=str+1;
	str[0]=(*p++)+2;
	str[1]=*(p+1);
	str[2]=p[1]+3;
	str[3]=p[0]+(str[2]-str[1]);
	printf("str[0]=%s\n",str[0]);
	printf("str[1]=%s\n",str[1]);
	printf("str[2]=%s\n",str[2]);
	printf("str[3]=%s\n",str[3]);
	return 0;
}

