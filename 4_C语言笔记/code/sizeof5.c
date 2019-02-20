/***************************************************************************
输出结果：
sizeof(a)=24,sizeof(b)=28,sizeof(c)=24,sizeof(d)=16,sizeof(union e)=8,sizeof(f)=48
***************************************************************************/
#include<stdio.h>

typedef struct a_t{
	char i[10];
	int j;
	char k[10];
} __attribute__((__packed__)) a;

typedef struct b_t{
	char i[10];
	int j;
	char k[10];
}b;

typedef struct c_t{
	int i;
	char j[10];
	char k[10];
}c;

typedef struct d_t{
        char a;
        int b;
        char c[5];
}d;

union e{
	char i[5];
	int j;
};

typedef struct f_t{
	int i;
	short * p;
	char k[32];
}f;

int main(void)
{
	printf("sizeof(a)=%d,sizeof(b)=%d,sizeof(c)=%d,sizeof(d)=%d,sizeof(union e)=%d,sizeof(f)=%d\n",sizeof(a),sizeof(b),sizeof(c),sizeof(d),sizeof(union e),sizeof(f));
	return 0;
}
