/***************************************************************************
输出结果：
Before sort, a[]=: 1 3 0 5 2 
After sort, a[]=: 0 1 2 3 5
***************************************************************************/
#include <stdio.h>

void sort(int a[], int len)
{
	int i,j,k,temp;

	for(i=0; i<len-1; i++)
	{
		k=i;
		for(j=i+1; j<len; j++)
		{
			if(a[k]>a[j])
				k=j;
		}

		if(i!=k)
		{
			temp = a[i];
			a[i] = a[k];
			a[k] = temp;
		}
	}
	return ;
}

int main(void)
{
	int i;
	int a[5] = { 1, 3, 0, 5, 2};

	printf("Before sort, a[]=: ");
	for(i=0; i<5; i++)
		printf("%d ",a[i]);
	printf("\n");

	sort(a,5);

	printf("After sort, a[]=: ");
	for(i=0; i<5; i++)
		printf("%d ",a[i]);
	printf("\n");

	return 0;
}
