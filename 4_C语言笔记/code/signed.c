/***************************************************************************
输出结果：
p=b63b3e94, *p=92
p+1=b63b3e95, *(p+1)=74
p+2=b63b3e96, *(p+2)=96
p+3=b63b3e97, *(p+3)=98

p=b63b3e94, *p=146
p+1=b63b3e95, *(p+1)=116
p+2=b63b3e96, *(p+2)=150
p+3=b63b3e97, *(p+3)=152
ffffff81

***************************************************************************/
#include<stdio.h>

int main(void)
{
	unsigned int m = 0x98967492;
	unsigned char *p=(char *)&m;
	printf("p=%x, *p=%02x\n",p,*p);
	printf("p+1=%x, *(p+1)=%02x\n",p+1,*(p+1));		
	printf("p+2=%x, *(p+2)=%02x\n",p+2,*(p+2));
	printf("p+3=%x, *(p+3)=%02x\n",p+3,*(p+3));
	printf("\n");
	p=(char *)&m;
	printf("p=%x, *p=%d\n",p,*p);
	printf("p+1=%x, *(p+1)=%d\n",p+1,*(p+1));		
	printf("p+2=%x, *(p+2)=%d\n",p+2,*(p+2));
	printf("p+3=%x, *(p+3)=%d\n",p+3,*(p+3));

	char n=-127;
	printf("%x\n",n);
	return 0;
}
