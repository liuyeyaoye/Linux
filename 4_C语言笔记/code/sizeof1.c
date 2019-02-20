/***************************************************************************
环境：Linux 4.4.0-34-generic #53~14.04.1-Ubuntu SMP Wed Jul 27 16:56:40 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
输出结果：
unsigned =4
short =2
int * =8
int =4
long =8
signed long=8
unsigned long=8
long long =8
float =4
***************************************************************************/
#include<stdio.h>
int main(void)
{
	printf("unsigned =%ld\n",sizeof(unsigned));
	printf("short =%ld\n",sizeof(short)); 
	printf("int * =%ld\n",sizeof(int *));
	printf("int =%ld\n",sizeof(int));
	printf("long =%ld\n",sizeof(long));
	printf("signed long=%ld\n",sizeof(signed long));
	printf("unsigned long=%ld\n",sizeof(unsigned long));
	printf("long long =%ld\n",sizeof(long long int)); 
	printf("float =%ld\n",sizeof(float));
	return 0;
}
