/*
编译： g++ -o test test.cpp listStack.cpp -std=c++11
输出结果：
stack is empty
val=3
val=2
val=1
stack is empty
*/

#include "listStack.h"

using namespace std;

int main(void)
{
	Stack stack;
	if(stack.is_empty()==1)
		cout<<"stack is empty"<<endl;

	stack.push(1);
	stack.push(2);
	stack.push(3);

	int val=stack.pop();
	cout<<"val="<<val<<endl;

	val=stack.pop();
	cout<<"val="<<val<<endl;

	val=stack.pop();
	cout<<"val="<<val<<endl;

	if(stack.is_empty()==1)
		cout<<"stack is empty"<<endl;

	return 0;
}


