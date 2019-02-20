/***************************************************************************
输出结果：
1  2  3  4  5  0  6  7  8  9  0  9  8  7  0  6  5  4  0  3  2  1  0  
1  2  3  4  5  0  6  7  6  7  8  9  8  7  0  6  5  4  0  3  2  1  0
***************************************************************************/
#include<stdio.h>

char * mystrncpy(char *dest, char *src, int n)
{
	char *p=dest;

	if(dest==NULL || src==NULL){
		return NULL;
	}

	if(dest==src){
		return src;
	}

	if( (dest>src) && (dest<(src+n)) ){
		dest = dest+n-1;
		src = src+n-1;
		while(n--){
			*dest-- = *src--;
		}
	}
	else{
		while(n--){
			*dest++ = *src++;
		}
	}

	return p;
}

int main(void)
{
	char buf[23]={1,2,3,4,5,'\0',6,7,8,9,'\0',9,8,7,'\0',6,5,4,'\0',3,2,1,'\0'};
	char *p1=buf;
	char *p2=buf;
	int i;

	for(i=0;i<23;i++){
		printf("%d  ",buf[i]);
	}
	printf("\n");

	mystrncpy(p1+8,p1+6,3);
	for(i=0;i<23;i++){
		printf("%d  ",buf[i]);
	}
	printf("\n");

	return 0;
}




