/***************************************************************************
输出结果：
max=2
abc, 1
d, 2
***************************************************************************/
#include<stdio.h>

void CalcCharCount(char * str)
{
	int i=0, j=0, max=0, flag=0, flag2=0;
	char * temp = str;
	char str1[27] = "abcdefghijklmnopqrstuvwxyz";
	int num[26]={0};

	if(str==NULL || *str=='\0')	return;

	while(*temp != '\0'){
		if(*temp>='a' && *temp<='z'){
			for(i=0; i<26; i++){
				if(*temp == str1[i]){
					num[i]++;
					max = (max>num[i])?max:num[i];
					break;
				}
			}
		}
		temp++;
	}

	printf("max=%d\n",max);

	for(j=1; j<=max; j++){
		for(i=0; i<26; i++){
			if(num[i]==j){
				printf("%c",str1[i]);
				flag=1;
			}
		}
		if(flag){
			flag=0;
			flag2=1;
			printf(", %d\n",j);
		}
	}
	if(!flag2){
		printf("no matched\n");
	}
}

int main(void)
{
	char * str1 = "MMMabcdd";
	CalcCharCount(str1);
	return 0;
}
