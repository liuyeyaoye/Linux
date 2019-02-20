/***************************************************************************
输出结果：
test1 = 40
test1[0] = 8
test1[1] = 8
***************************************************************************/

#include <stdio.h>
int main(void)
{
	static char * test1[] = {"aa","bbb","cccc","ddddd","eeeeee"};
	printf("test1 = %ld\n",sizeof(test1));
	printf("test1[0] = %ld\n",sizeof(test1[0]));
	printf("test1[1] = %ld\n",sizeof(test1[1]));
	return 0;
}
