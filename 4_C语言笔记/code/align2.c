/***************************************************************************

编译环境：gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.4)
编译方式：gcc -o align2 align2.c
运行方式：./align2
程序功能：测试结构体的偏移大小
输出结果：
sizeof(struct stu)=12
sizeof(struct stu1)=8
sizeof(struct student3)=32
sizeof(struct student5)=8
sizeof(struct stu6)=6
sizeof(struct stu8)=12
***************************************************************************/
#include <stdio.h>

struct stu {
	char c1;
	int i;
	char c2;
};

struct stu1 {
	char c1;
	char c2;
	int i;
};

struct student3 {
	int num;
	char c[13];
}__attribute__((aligned));

struct student5 {
	int num;
	char c;
}__attribute__((aligned(4)));

struct stu6 {
	char a;
	short b;
	char c[2];
};

struct stu7 {
	char a;
	short b;
	int c;
};

struct stu8 {
	char a;
	int c;
	short b;
};

int main(void)
{
	int a,b,c,d,e,f;
	a=sizeof(struct stu);
	b=sizeof(struct stu1);
	c=sizeof(struct student3);
	d=sizeof(struct student5);
	e=sizeof(struct stu6);
	f=sizeof(struct stu8);
	printf("sizeof(struct stu)=%d\nsizeof(struct stu1)=%d\nsizeof(struct student3)=%d\nsizeof(struct student5)=%d\nsizeof(struct stu6)=%d\nsizeof(struct stu8)=%d\n",
			a,b,c,d,e,f);
	return 0;
}
