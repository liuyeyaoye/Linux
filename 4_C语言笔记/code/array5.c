/***************************************************************************
输出结果：
ptr1=140735172994148,ptr2=140735172994160,ptr1+2=140735172994156,ptr2-ptr1=3,ptr1-ptr2=-3
***************************************************************************/
#include<stdio.h>

int main(void)
{
	unsigned int a[6]={1,2,3,4,5,6};
	unsigned int *ptr1,*ptr2;
	ptr1=&a[1];
	ptr2=&a[4];
	printf("ptr1=%ld,ptr2=%ld,ptr1+2=%ld,ptr2-ptr1=%d,ptr1-ptr2=%d\n",ptr1,ptr2,ptr1+2,ptr2-ptr1,ptr1-ptr2);

}
