
```c

#include<stdio.h>
#include<assert.h>

int strlen1(const char * str)
{
	assert(str!=NULL);
	int len=0;
	while(*str != '\0'){
		len++;
		str++;
	}
	return len;
}

int strlen2(const char * str)
{
	assert(str!=NULL);
	const char *temp=str;
	while(*temp != '\0'){
		temp++;
	}
	return temp-str;
}

int strlen3(const char * str)
{
	assert(str!=NULL);
	if(*str == '\0')
		return 0;
	else
		return (1+strlen3(++str));
}

int main(void)
{
	char *s1="abc";
	char *s2=" ";
	char *s3="abcde";
	int m1,m2,m3;
	m1=strlen1(s1);
	m2=strlen2(s2);
	m3=strlen3(s3);
	printf("%d %d %d\n",m1,m2,m3);

	return 0;
}

```

