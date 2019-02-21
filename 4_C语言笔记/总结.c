


```c

C语言程序总结：

1. 关于几个排序算法的原理

冒泡排序：
(1) 原理：对数组中的n个元素，依次比较相邻2个元素的大小，如果第2个比第1个大，则交换位置。这样比较一轮下来，最后一个元素是最大的。然后对剩下的n-1个元素，执行相同的操作。
(2) 实现
void sort(int a[], int n)
{
	int i=0,j=0,temp=0;
	for(i=0; i<n; i++){
		for(j=1; j<n-i; j++){
			if(a[j]<a[j-1]){
				temp=a[j-1];
				a[j-1]=a[j];
				a[j]=temp;
			}
		}
	}
}



快速排序：
(1) 原理：从待排序的数组取出一个基准元素，通常是第1个。将待排序的元素分区，比基准元素大的放在右边，小的放在左边。对左右2个分区重复以上步骤。
(2) 实现：
void sort(int a[], int num, int n)
{
	if(num<n){
		int i=num,j=n-1,x=a[num];
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



插入排序：
(1) 原理：将数组分成2部分，一部分是有序的，一部分是无序的，然后从无序数组中取出一个数，插入到有序的部分。
(2) 实现
void sort(int a[], int n)
{
	int i,j,temp;
	for(i=1; i<n; i++){
		temp=a[i];//从无序数组中取出一个元素
		for(j=i; j>0 && a[j-1]>temp; j--){//将该元素跟有序数组的元素逐一比较
			a[j]=a[j-1];//移动有序数组的元素
		}
		a[j]=temp;//插入该元素到有序数组
	}
}


2. 实现 strcpy 和 memcpy 时，注意内存重叠，如果 src<des<src+n，使用n--复制。否则，使用n++复制。



```


