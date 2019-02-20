/***************************************************************************
输出结果：
6 7 5 8 4 9 1 
0 1 4 5 6 7 8
***************************************************************************/
#include<stdio.h>

void quick(int*a, int num, int n)
{
	if(num<n){
		int i=num,j=n,x=a[num];
		while(i<j){
			while(i<j && a[j]>=x)
				j--;
			if(i<j)
				a[i++]=a[j];
			while(i<j && a[i]<=x)
				i++;
			if(i<j)
				a[j--]=a[i];
		}
		a[i]=x;
		quick(a,num,i-1);
		quick(a,i+1,n);
	}
}

int main(void)
{
	int i;
	int a[7]={6,7,5,8,4,9,1};
	for(i=0;i<7;i++)
		printf("%d ",a[i]);
	printf("\n");

	quick(a, 0, 7);

	for(i=0;i<7;i++)
		printf("%d ",a[i]);
	printf("\n");
	return 0;
}
