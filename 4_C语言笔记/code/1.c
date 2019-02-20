//求一个二进制正整数中1的个数

#include<stdio.h>

int num(int n)
{
	int count=0;
	while(n){
		n=n&(n-1);
		count ++;
	}
	return count;
}


int main(void)
{
	int i;
	i=num(4);
	printf("i=%d\n",i);

	i=num(256);
	printf("i=%d\n",i);

	i=num(999);
	printf("i=%d\n",i);

	return 0;
}

