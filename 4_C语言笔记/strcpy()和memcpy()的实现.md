

```c

一、strcpy和memcpy的区别
1、strcpy的类型为char *，只能复制字符串。memcpy的类型为void *，可以复制任何类型。
2、strcpy复制字符串时，遇到'\0'后停止复制。memcpy只会复制count个数据。


二、实现


#include<stdio.h>
#include<assert.h>

void *Mymemcpy(void *des, const void *src, int n)
{
	assert((des!=NULL) && (src!=NULL));

	char *dest=(char *)des;
	char *srce=(char *)src;

	if( (dest>srce) && (dest<(src+n))){
		dest=dest+n-1;
		srce=srce+n-1;
		while(n--){
			*dest-- = *srce--;
		}
	}
	else{
		while(n--){
			*dest++ = *srce++;
		}
	}
	return dest;
}

char *strcpy(char *des, const char *src)
{
	assert((des != NULL) && (src != NULL));

	char *temp=des;
	
	while( (*temp++ = *src++) != '\0');
	
	return temp;
}

int main(void)
{
	char s1[]="abcd";
	char *s2="cde";
	char s3[]="abcdef";

	strcpy(s1,s2);
	Mymemcpy(s3+2,s3,4);
	printf("s1=%s,s3=%s\n",s1,s3);
	Mymemcpy(s3,s3+2,4);
	printf("s1=%s,s3=%s\n",s1,s3);
	return 0;
}
```


