/***************************************************************************
输出结果：
1!=1
2!=2
3!=6
4!=24
5!=120
6!=720
7!=5040
8!=40320
9!=362880
***************************************************************************/
#include<stdio.h>

#define MAX 5
int data[MAX];

int *calcfact(int n)
{
	int i,j,k,r;
	int digit=1;
	for(i=1;i<MAX+1;i++)
		data[i]=0;

	data[0]=1;
	data[1]=1;

	for(i=1;i<n+1;i++){
		for(j=1;j<digit+1;j++)
			data[j]*=i;

		for(j=1;j<digit+1;j++){
			if(data[j]>10){
				for(r=1;r<digit+1;r++){
					if(data[digit]>10)
						digit++;

					data[r+1] += data[r]/10;
					data[r]=data[r]%10;
				}
			}
		}
		printf("%d!=",i);
		for(k=digit;k>0;k--)
			printf("%d",data[k]);
		printf("\n");
	}
	return data;
}

int main(void)
{
	int n=9;
	calcfact(n);
	return 0;
}
