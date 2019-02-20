
```c
一、实现C语言字符串 atoi 函数

1、功能：将一个字符串转换成整型。

2、原型： int atoi(const char *str);

3、字符转换成整型的原理
char str = '1';
int i,j;
i=str;
j='1'-'0';
结果： str='1',i=49,j=1;
分析：字符'1'的ASCII码为49，字符'0'的ASCII码为48。
字符在内存中存储的是它的ASCII码对应的二进制。
ASCII码占1个字节。
ASCII码是十进制的。
字符'1'在内存中的存储为(占1个字节)： 0011 0001
数字1在内存中的存储为(占4个字节)：   0x00000001

4、转义字符
八进制：'\d'，'\dd'，'\ddd'，表示1到3位八进制的ASCII码。
十六进制：'\xh'，'\xhh'，'\xhhh'，表示1到3位十六进制的ASCII码。
其他：'\n'表示换行符， '\0'的ASCII码为0，表示空字符。

5、实现atoi()


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
	int i,j,k,l,m,n;
	i=atoi(a);
	j=atoi(b);
	k=atoi(c);
	l=atoi(d);
	m=atoi(e);
	n=atoi(f);
	printf("a=%s,b=%s,c=%s,d=%s,e=%s,f=%s\n",a,b,c,d,e,f);
	printf("i=%d,j=%d,k=%d,l=%d,m=%d,n=%d\n",i,j,k,l,m,n);
	return 0;
}
```

