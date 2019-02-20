/***************************************************************************
输出结果：
Before sort,a[] =  9    9    8    6    7    5    2    3    1    0    
After sort,a[] =  0    1    2    3    5    6    7    8    9    9
***************************************************************************/
#include<stdio.h>

void swap(int *a, int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void print(int a[], int len)
{
	int i;
	for(i=0; i<len; i++)
		printf("%d    ",a[i]);
	printf("\n");
}

void quicksort(int a[], int len, int begin, int end)
{
	int i,j;
	if(begin<end)
	{	
		i=begin+1;
		j=end;

		while(i<j)
		{
			if(a[i]>a[begin])
			{
				swap(&a[i],&a[j]);
				j--;
			}
			else
				i++;
		}

		if(a[i]>=a[begin])
			i--;

		swap(&a[i],&a[begin]);
		quicksort(a,len,begin,i);
		quicksort(a,len,j,end);
	}
}

int main(void)
{
	int a[11] = {9, 9, 8, 6, 7, 5, 2, 3, 1, 0, 4};
	int i;

	printf("Before sort,a[] =  ");
	print(a,10);

	quicksort(a, 11, 0, 9);

	printf("After sort,a[] =  ");
	print(a,10);

	return 0;
}
