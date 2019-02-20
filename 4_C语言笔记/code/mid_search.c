/***************************************************************************
输出结果：
result=1
***************************************************************************/
#include<stdio.h>

int search(int n, int a[], int len)
{
	int mid=0, min=0, max=len-1, result=0;

	while(min<=max){
		mid=(min+max)/2;
		if(n==a[mid]){
			result=1;
			break;
		}
		else if(n>a[mid]){
			min=mid+1;
		}
		else{
			max=mid-1;
		}		
	}
	return result;
}


int main(void)
{
	int a[1]={9};
	int b;
	b=search(9, a, 1);
	printf("result=%d\n",b);
	return 0;
}
