#include "listStack.h"

using namespace std;

void Stack::push(int val)
{
	Node * ptemp=new Node();
	ptemp->data=val;
	length++;

	if(head==NULL){
		head=ptemp;
		pcur=ptemp;
		ptemp->next=NULL;
	}
	else{
		ptemp->next=pcur;
		pcur=ptemp;
	}
}

int Stack::pop(void)
{
	if(is_empty()==1){
		cout<<"stack is empty..."<<endl;
		return 0;
	}

	length--;
	int val=pcur->data;
	Node* ptemp=pcur;
	pcur=pcur->next;
	delete(ptemp);

	return val;
}

int Stack::len(void)
{
	return length;
}

bool Stack::is_empty(void)
{
	if(length==0)
		return 1;
	else
		return 0;
}

void Stack::clear(void)
{
	if(length>0)
		pop();
}

