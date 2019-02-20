/***************************************************************************
输出结果：
n=5, pbuf=zzzzz
n=5, pbuf=
***************************************************************************/
#include<stdio.h>
#include<string.h>

int main(void)
{
	char buf[100]={0};
	char *pbuf=buf;
	int n;
	n=sprintf(pbuf, "zzzzz");
	printf("n=%d, pbuf=%s\n",n,buf);

	memset(pbuf, 0, sizeof(buf));
	printf("n=%d, pbuf=%s\n",n,buf);
	return 0;
}
