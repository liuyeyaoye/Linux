/***************************************************************************
输出结果：
Before sort,a[] =  3 4 12 0 5 
After sort, a[] =  0 3 4 5 12 
num=4
***************************************************************************/
#include<stdio.h>

int num=0;
int main(void)
{
	int i,j,temp;
	int a[5]={3, 4, 12, 0, 5};

	printf("Before sort,a[] =  ");
	for(i=0; i<5; i++)
		printf("%d ",a[i]);
	printf("\n");

	for(i=0; i<5; i++) {
		for(j=i+1; j<5; j++) {
			if(a[i]>a[j]) {
				temp=a[i];
				a[i]=a[j];
				a[j]=temp;
				++num;
			}	
		}
	}

	printf("After sort, a[] =  ");
	for(i=0; i<5; i++)
		printf("%d ",a[i]);
	printf("\n");
	printf("num=%d\n",num);

	return 0;
}
