#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define LEN 32

typedef struct tree{
	char str[LEN];
	int count;
	struct tree * left;
	struct tree * right;
} node;

void insert(node **tree, char *val)
{
	node *temp=NULL;
	if( !(*tree) ){
		temp=(node*)malloc(sizeof(node));
		temp->left=NULL;
		temp->right=NULL;
		strcpy(temp->str, val);
		temp->count=1;
		*tree=temp;
		return;
	}

	if( strcmp(val, (*tree)->str) <0 ){
		insert(&(*tree)->left, val);
	}else if( strcmp(val, (*tree)->str) >0){
		insert(&(*tree)->right, val);
	}else{
		(*tree)->count++;
	}
}

void deltree(node *tree)
{
	if(tree){
		deltree(tree->left);
		deltree(tree->right);
		free(tree);
	}
}

void print_midorder(node *tree)
{
	if(tree){
		print_midorder(tree->left);
		printf("[%s\t] count:[%d]\n",tree->str, tree->count);
		print_midorder(tree->right);
	}
}

int main(void)
{
	node *root, *tmp;
	root=NULL;

	insert(&root, "hello");
	insert(&root, "hey");
	insert(&root, "hello");
	insert(&root, "ok");
	insert(&root, "hey");
	insert(&root, "hey");
	insert(&root, "hey");

	printf("mid order display:\n");
	print_midorder(root);

	return 0;
}
