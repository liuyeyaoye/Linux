/***************************************************************************
输出结果：
5 6 4 7 3 8 2 9 1 1 
1 1 2 4 3 5 6 7 8 9
***************************************************************************/
#include<stdio.h>

void sort(int a[], int n)
{
	int i=0,j=n-1,k=a[i];
	if(n-1>0){
		while(i<j){
			while(i<j && a[j]>k)
				j--;
			if(i<j)
				a[i++]=a[j];
			while(i<j && a[i]<k)
				i++;
			if(i<j)
				a[j--]=a[i];
		}
		a[i]=k;
		sort(a,i-1);
		sort(a+i+1, n-i-1);
	}
}

int main(void)
{
	int i;
	int a[10]={5,6,4,7,3,8,2,9,1,1};
	for(i=0;i<10;i++)
		printf("%d ",a[i]);
	printf("\n");

	sort(a,10);

	for(i=0;i<10;i++)
		printf("%d ",a[i]);
	printf("\n");
	return 0;
}
