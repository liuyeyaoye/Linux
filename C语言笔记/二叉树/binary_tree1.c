#include<stdio.h>
#include<stdlib.h>

/**
reference:https://www.cnblogs.com/landpack/p/4783120.html
二叉树的遍历：
前序遍历二叉树：根左右，先根结点，后左结点，最后右结点；
中序遍历二叉树：左根右，先左结点，后根结点，最后右结点；
后序遍历二叉树：左右根，先左结点，后右结点，最后根结点；
*/

typedef struct binary_tree{
	int data;
	struct binary_tree * left;
	struct binary_tree * right;
} node;

void insert(node **tree, int val)
{
	node *temp=NULL;
	if(! (*tree) ){
		temp=(node*)malloc(sizeof(node));
		temp->left=NULL;
		temp->right=NULL;
		temp->data=val;
		*tree=temp;
		return;
	}

	if(val< (*tree)->data){
		insert(&(*tree)->left, val);
	}
	else if(val> (*tree)->data){
		insert(&(*tree)->right, val);
	}
}

void print_preorder(node *tree)
{
	if(tree){
		printf("%d  ",tree->data);
		print_preorder(tree->left);
		print_preorder(tree->right);
	}
}

void print_midorder(node *tree)
{
	if(tree){
		print_midorder(tree->left);
		printf("%d  ",tree->data);
		print_midorder(tree->right);
	}
}

void print_backorder(node *tree)
{
	if(tree){
		print_backorder(tree->left);
		print_backorder(tree->right);
		printf("%d  ",tree->data);
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

int main(void)
{
	node *root, *tmp;
	root=NULL;

	insert(&root, 9);
	insert(&root, 4);
	insert(&root, 15);
	insert(&root, 6);
	insert(&root, 12);
	insert(&root, 17);
	insert(&root, 2);

	printf("pre order display:   ");
	print_preorder(root);
	printf("\n");

	printf("mid order display:   ");
	print_midorder(root);
	printf("\n");

	printf("back order display:  ");
	print_backorder(root);
	printf("\n");

	deltree(root);

	return 0;
}
