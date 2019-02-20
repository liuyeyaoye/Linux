#include<stdio.h>

//有序数组的二分查找法
int midSearch(int key, int length, int * array)
{
	int mid=0;
	int start=0;
	int end=length-1;

	while(start<=end){
		mid=(end-start)/2+start;
		if(key<array[mid]){
			end=mid-1;
		}
		else if(key>array[mid]){
			start=mid+1;
		}
		else{
			return mid;
		}
	}
	return -1;
}



int main(void)
{
	int a[5]={1,4,6,8,9};
	int b=midSearch(9,5,a);
	printf("result=%d\n",b);
	return 0;
}


