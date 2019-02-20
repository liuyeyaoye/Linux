/***************************************************************************

编译环境：gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.4)
编译方式：gcc -o strcpy strcpy_memcpy.c
运行方式：./strcpy
程序功能：实现函数 strcpy() 和 memcpy().
输出结果：
s1=cde,s3=ababcd
s1=cde,s3=abcdcd
***************************************************************************/

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
