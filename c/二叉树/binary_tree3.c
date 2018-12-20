#include<stdio.h>
#include<stdlib.h>

//reference:https://www.cnblogs.com/landpack/p/4860652.html
typedef struct node{
	int data;
	struct node *link[2];
} Node;

typedef struct tree{
	struct node *root;
} Tree;

Tree * init_tree(void)
{
	Tree *temp = (Tree *)malloc(sizeof(Tree));
	temp->root=NULL;
	return temp;
}

Node * make_node(int data)
{
	Node *temp = (Node*)malloc(sizeof(Node));
	temp->link[0]=NULL;
	temp->link[1]=NULL;
	temp->data=data;
	return temp;
}

Node * insert_recursive(Node *root, int data)
{
	if(root==NULL){
		root=make_node(data);
	}else if(root->data == data){
		return root;
	}else{
		int dir=root->data<data;
		root->link[dir]=insert_recursive(root->link[dir], data);
	}
	return root;
}

void insert(Tree *tree, int data)
{
	tree->root = insert_recursive(tree->root, data);
}

void print_inorder_recursive(Node *root)
{
	if(root){
		print_inorder_recursive(root->link[0]);
		printf("data:%d\n",root->data);
		print_inorder_recursive(root->link[1]);
	}
	return;
}

void print_inorder(Tree * tree)
{
	print_inorder_recursive(tree->root);
	return;
}


int main(void)
{
	Tree *tree=init_tree();
	insert(tree, 9);
	insert(tree, 7);
	insert(tree, 3);
	insert(tree, 19);
	insert(tree, 29);
	insert(tree, 82);
	insert(tree, 1);

	print_inorder(tree);
	return 0;
}
