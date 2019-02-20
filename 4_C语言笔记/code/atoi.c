/***************************************************************************
输出结果：
a= 12345.67,  b=-123 34,  c=12344,  d= ,  e=- 1,  f= +111 .
aa=12345,     bb=-123,    cc=12344, dd=0, ee=0,   ff=111
***************************************************************************/

#include<stdio.h>

int atoi(const char *str)
{
	int num=0;
	int flag=1;
	
	if( (str == NULL) || (*str == '\0') ){
		return 0;
	}
	
	while(*str == ' '){
		str++;
	}

	if(*str == '\0'){
		return 0;
	}
	else if(*str == '-'){
		flag = -1;
		str++;
	}
	else if(*str == '+'){
		flag=1;
		str++;
	}

	while(*str != '\0'){
		if( (*str >= '0') && (*str <= '9') ){
			num = num*10 + (flag)*(*str - '0');
			if( ( (-1 == flag) && (num<(signed int)0x80000000) ) || ( (1 == flag) && (num>0x7fffffff) )){
				return 0;
			}
			str++;
		}
		else{
			break;
		}
	}

	return num;
}

int main(void)
{
	char *a=" 12345.67";
	char *b="-123 34";
	char *c="12344";
	char *d=" ";
	char *e="- 1";
	char *f=" +111 .";
	int aa,bb,cc,dd,ee,ff;
	aa=atoi(a);
	bb=atoi(b);
	cc=atoi(c);
	dd=atoi(d);
	ee=atoi(e);
	ff=atoi(f);
	printf("a=%s,  b=%s,  c=%s,  d=%s,  e=%s,  f=%s\n",a,b,c,d,e,f);
	printf("aa=%d,     bb=%d,    cc=%d, dd=%d, ee=%d,   ff=%d\n",aa,bb,cc,dd,ee,ff);

	return 0;
}
