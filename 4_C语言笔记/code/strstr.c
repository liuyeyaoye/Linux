/***************************************************************************
输出结果：
n=19
***************************************************************************/
#include<stdio.h>
#include<string.h>

int strstrnum(const char * str1, const char * str2)
{
	int n=0;
	int len=strlen(str2);
	char *temp=str1;
	if(str1==NULL || str2==NULL || len<=0){
		return 0;
	}

	while( (*temp != '\0') && (temp=strstr(temp,str2)) ){
		++n;
		temp += len;
	}
	return n;
}

int main(void)
{
	char *s1="zzzzz is a factory on producing mobile phone and pos.zzzzz is on shenzhen China.zzzzz is developing with our works";
	char *s2=" ";
	int n = strstrnum(s1,s2);
	printf("n=%d\n",n);
	return 0;
}
