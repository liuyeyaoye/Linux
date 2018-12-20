#include<stdio.h>
#include<stdlib.h>

//reference:https://www.cnblogs.com/landpack/p/4862847.html
typedef struct node{
	int data;
	struct node *link[2];
} Node;

typedef struct tree{
	struct node *root;
} Tree;

Tree * init_tree(void)
{
	Tree *temp=(Tree*)malloc(sizeof(Tree));
	temp->root=NULL;
	return temp;
}

Node * make_node(int data)
{
	Node *temp=(Node*)malloc(sizeof(Node));
	temp->link[0]=NULL;
	temp->link[1]=NULL;
	temp->data=data;
	return temp;
}

void insert(Tree *tree, int data)
{
	if(tree->root==NULL){
		tree->root=make_node(data);
	}else{
		Node *it=tree->root;
		int dir;
		for(;;){
			dir=it->data<data;
			if(it->data==data){
				return;
			}else if(it->link[dir]==NULL){
				break;
			}
			it=it->link[dir];
		}
		it->link[dir]=make_node(data);
	}
}

void print_inorder_recursive(Node *root)
{
	if(root){
		print_inorder_recursive(root->link[0]);
		printf("data:%d\n",root->data);
		print_inorder_recursive(root->link[1]);
	}
}

void print_inorder(Tree *tree)
{
	print_inorder_recursive(tree->root);
}

int main(void)
{
	Tree *tree=init_tree();
	insert(tree,6);
	insert(tree,7);
	insert(tree,5);
	insert(tree, 8);

	print_inorder(tree);
	return 0;
}
