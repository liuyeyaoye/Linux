#include<stdio.h>

//reference: https://www.cnblogs.com/landpack/p/4781579.html
void quick_sort(int *a, int n)
{
	int i,j,p,temp;

	if(n<2)	return;//如果只有一个数，那么就不需要进行比较了。

	p=a[n/2];
	//p为数组中间的数，对数组排序，让p左边的数都小于p，p右边的数都大于p。

	for(i=0,j=n-1; ;i++,j--){
		while(a[i]<p){
			i++;
		}

		while(a[j]>p){
			j--;
		}
		
		if(i>=j)	break;
		
		temp=a[j];
		a[j]=a[i];
		a[i]=temp;
	}
	
	quick_sort(a,i);//递归的处理数组的前部分
	quick_sort(a+i,n-i);//递归的处理数组的后部分
}


int main(void)
{
	#define LEN 4
	int a[LEN] = {0,-99,89,-3};
	int i;

	printf("before sort: ");
	for(i=0;i<LEN;i++){
		printf("%d ",a[i]);
	}
	printf("\n");

	quick_sort(a,LEN);

	printf("after sort:  ");
	for(i=0;i<LEN;i++){
		printf("%d ",a[i]);
	}
	printf("\n");

	return 0;
}
