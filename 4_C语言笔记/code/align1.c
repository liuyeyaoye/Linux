/***************************************************************************

编译环境：gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.4)
编译方式：gcc -o align1 align1.c
运行方式：./align1
程序功能：测试结构体的偏移大小
输出结果：
sizeof(struct student)=8
sizeof(struct student2)=5
sizeof(struct student3)=16
sizeof(struct student4)=8
sizeof(struct student5)=16
sizeof(struct student6)=16
sizeof(struct student7)=6
sizeof(struct student8)=32
sizeof(struct student9)=16
sizeof(ptr)=8
char:1
short:2
int:4
long:8
float:4
double:8
long long:8
***************************************************************************/
#include<stdio.h>

struct student{
	int age;
	char c;
};

struct student2{
	int age;
	char c;
}__attribute__((packed));

struct student3 {
	int num;
	char c;
}__attribute__((aligned));

struct student4 {
	char c;
	int num;
};

struct student5 {
	char c[9];
	int num;
};

struct student6 {
	int num;
	char c;
}__attribute__((aligned(16)));

#pragma pack (2)
struct student7 {
	int num;
	char c;
};
#pragma pack () 

struct student8 {
	int num;
	char c[13];
}__attribute__((aligned));

struct student9 {
	char num[2];
	int * a;
};

int main(void)
{
	int * ptr =NULL;
	int a,b,c,d,e,f,g,h,i,j,k,l,m,n;
	a=sizeof(struct student);
	c=sizeof(struct student2);
	e=sizeof(struct student3);
	g=sizeof(struct student4);
	i=sizeof(struct student5);
	k=sizeof(struct student6);
	l=sizeof(struct student7);
	m=sizeof(struct student8);
	n=sizeof(struct student9);
	printf("sizeof(struct student)=%d\n",a);
	printf("sizeof(struct student2)=%d\n",c);
	printf("sizeof(struct student3)=%d\n",e);
	printf("sizeof(struct student4)=%d\n",g);
	printf("sizeof(struct student5)=%d\n",i);
	printf("sizeof(struct student6)=%d\n",k);
	printf("sizeof(struct student7)=%d\n",l);
	printf("sizeof(struct student8)=%d\n",m);
	printf("sizeof(struct student9)=%d\n",n);
	printf("sizeof(ptr)=%ld\n",sizeof(ptr));
	printf("char:%ld\nshort:%ld\nint:%ld\nlong:%ld\nfloat:%ld\ndouble:%ld\nlong long:%ld\n",
		sizeof(char),sizeof(short),sizeof(int),sizeof(long),sizeof(float),sizeof(double),sizeof(long long));
	
	return 0;
}
