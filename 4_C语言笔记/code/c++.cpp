/***************************************************************************
编译： g++ -o c++ c++.cpp -std=c++11
输出结果：
test99  4
test81  test88
6
s
o
m
e
t
h
i
n
g
***************************************************************************/
#include<iostream>
#include<string>
#include<vector>

using namespace std;

int main(int argc, char **argv)
{
	int b=99;
	const int & a=99;
	string str1("test81");
	string str2 = "test88";
	cout<<"test"<<a<<"  "<<sizeof(unsigned)<<endl;
	cout<<str1<<"  "<<str2<<endl;
	cout<<str2.size()<<endl;

	string str3("something");
	for(auto i : str3)
		cout<<i<<endl;

	return 0;
}
