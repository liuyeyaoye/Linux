/***************************************************************************
编译： g++ -o vector vector.cpp -std=c++11
输出结果：
3
4
***************************************************************************/
#include<iostream>
#include<vector>

using namespace std;

int main(int argc, char **argv)
{
	vector<char> i;
	for(vector<char>::size_type j=0;j!=3;j++)
		i.push_back(j);

	cout<<i.size()<<endl;
	cout<<i.capacity()<<endl;

	return 0;
}
