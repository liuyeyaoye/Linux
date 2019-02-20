/***************************************************************************
输出结果：
is little endian
***************************************************************************/
#include<stdio.h>

/*int isBigEndian(void)
{
	unsigned int m=0x98765432;
	unsigned char *p=(unsigned char *)&m;

	if(*p == 0x32)
		return 0;//is little endian
	else if(*p == 0x98)
		return 1;//is big endian
	return -1;
}*/

//联合体union的存放顺序是所有成员都从低地址开始存放，而且所有成员共享存储空间。
int isBigEndian(void)
{
	union Temp{
		unsigned int a;
		unsigned char b;
	}temp;

	temp.a = 0x98765432;

	if(temp.b == 0x32)
		return 0;//is little endian
	else if(temp.b == 0x98)
		return 1;//is big endian
	return -1;
}

int main(void)
{
	if(isBigEndian()==1)
		printf("is big endian\n");
	else if(isBigEndian()==0)
		printf("is little endian\n");
	return 0;
}
