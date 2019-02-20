
#include "list.h"

//https://blog.csdn.net/zxxyyxf/article/details/6772764
//单链表逆置
node * inserveList(node * head)
{
	node *p1, *p2, *p3;

	if(head==NULL || head->next==NULL){
		return head;
	}

	p1=head;
	p2=head->next;

	while(p2){
		p3=p2->next;
		p2->next=p1;
		p1=p2;
		p2=p3;
	}

	head->next=NULL;
	head=p1;

	return head;
}



//合并2个有序链表（普通算法）
node * mergeList1(node * head1, node * head2)
{
	node *p1, *p2, *temp=NULL, *head=NULL, *cur=NULL;
	p1=head1;
	p2=head2;

	while(p1!=NULL && p2!=NULL){
		if(p1->data < p2->data){
			temp=p1;
			p1=p1->next;
		}
		else{
			temp=p2;
			p2=p2->next;
		}

		if(head==NULL){
			cur=temp;
			head=temp;
		}
		else{
			cur->next=temp;
			cur=temp;
		}
	}
	temp=(p1?p1:p2);
	while(temp!=NULL){
		cur->next=temp;
		cur=cur->next;
		temp=temp->next;
	}
	cur->next=NULL;
	
	return head;
}

//合并2个有序链表（递归算法）
node * mergeList2(node * head1, node * head2)
{
	if(head1==NULL)
		return head2;

	if(head2==NULL)
		return head1;

	node *head=NULL;
	if(head1->data <= head2->data){
		head=head1;
		head->next=mergeList2(head1->next, head2);
	}
	else{
		head=head2;
		head->next=mergeList2(head1, head2->next);
	}

	return head;
}


//判断单链表是否存在环
int isExistLoop(node *head)
{
	node *slow=head, *fast=head;

	while(fast && fast->next){
		slow = slow->next;
		fast = fast->next->next;
		if(slow==fast)	break;
	}

	return !(fast==NULL ||fast->next==NULL);
}

int main(void)
{
	return 0;
}

