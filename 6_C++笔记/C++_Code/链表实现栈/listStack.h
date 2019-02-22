#include<iostream>

class Node{
public:
	int data;
	Node * next;
};

class Stack{
private:
	Node * head;
	Node * pcur;
	int length;

public:
	Stack()
	{
		head=NULL;
		pcur=NULL;
		length=0;
	}

	void push(int val);//入栈
	int pop(void);//出栈
	int len(void);//判断栈长度
	bool is_empty(void);//判断栈是否为空
	void clear(void);//清空栈
};

