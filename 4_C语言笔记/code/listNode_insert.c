#include<stdio.h>
#include<stdlib.h>

typedef struct Node
{
	struct Node * next;
	int val;
}node;

int insert1(node* root, int newVal)
{
	node * prev;
	node * new;

	if(root == NULL)	return 0;

	while(root != NULL){
		if(root->val > newVal)	break;
		prev=root;
		root=root->next;
	}

	new=(node *)malloc(sizeof(node));
	if(new == NULL)	return 0;

	new->val=newVal;
	prev->next=new;
	new->next=root;
	return 1;
}

int insert2(node *root, int newVal)
{
	node* prev=NULL;
	node* new=NULL;

	if(root==NULL)	return 0;

	while( (root != NULL) && (root->val<newVal) ){
		prev=root;
		root=root->next;
	}

	new=(node*)malloc(sizeof(node));
	if(new==NULL)	return 0;

	new->val=newVal;
	prev->next=new;
	new->next=root;
	return 1;
}

int insert3(node **rootp, int newVal)
{
	node* new=NULL;

	if(*rootp ==NULL)	return 0;

	while( (*rootp != NULL) && ((*rootp)->val<newVal) ){
		rootp=&((*rootp)->next);
	}

	new=(node*)malloc(sizeof(node));
	if(new==NULL)	return 0;

	new->val=newVal;
	new->next=*rootp;
	*rootp=new;

	return 1;
}

int main(void)
{
	node n3={NULL, 5};
	node n2={&n3, 9};
	node n1={&n2, 1};
	node * root =&n1;
	
	insert1(root, 3);
	

	return 0;
}
