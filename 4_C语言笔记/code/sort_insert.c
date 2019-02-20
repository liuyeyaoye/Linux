/***************************************************************************
输出结果：
1 9 3 6 5 7 4 
1 3 4 5 6 7 9
***************************************************************************/
#include<stdio.h>

void insert(int *a, int n)
{
	int i,j,temp;
	for(i=1;i<n;i++){
		temp=a[i];
		for(j=i; j>0 && a[j-1]>temp; j--)
			a[j]=a[j-1];
		a[j]=temp;
	}
}

int main(void)
{
	int i;
	int a[7]={1,9,3,6,5,7,4};
	for(i=0;i<7;i++)
		printf("%d ",a[i]);
	printf("\n");
	insert(a,7);
	for(i=0;i<7;i++)
		printf("%d ",a[i]);
	printf("\n");
}
