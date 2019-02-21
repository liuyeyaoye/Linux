C++ Primer



```c
第I部分：C++基础知识



第1章、变量

0、当一个表达式存在带符号和不带符号的类型时，带符号的会自动转换成不带符号的，signed会自动转换成 unsigned。
编译器会在字符串的末尾处自动添加一个空字符'\0'。
字符数组在定义时要以'\0'结尾，否则对它使用strlen函数时会发生错误，strlen会沿着该字符串在内存中的位置继续寻找，直到遇到空字符才停下来。
下面的代码会出错，因为字符串没有以0结尾：
char ca[] = {'c', 'd', 'e'};
cout<<strlen(ca)<<endl;
但是strlen不会把空字符计算在长度之内。


1、 引用 &必须被初始化，未初始化的引用会编译报错。


2、 void * 指针： void *指针可以存放任意对象的地址，一个 void * 指针存放着一个地址，但是我们对该地址中是个什么类型的对象并不了解。
void * 常用于作为函数的输入和输出。不能直接操作 void * 指向的对象，因为我们不知道该对象是什么类型。


3、 引用不是一个对象，但是指针是一个对象。


4、 const 对象一旦创建后其值不再发生变化，不能对其进行赋值操作，所以 const 对象必须初始化。

(1)例如： const int bufsize = 512; 
编译器在编译过程中会寻找代码中所有用到 bufsize 的地方，将它替换成 512 。

(2) const 的作用域仅在本文件有效。如果想在其他文件使用本文件的 const 变量，必须如下做：
本文件中定义 const 变量： extern const int bufsize = 512;
其他文件中声明这个 const 变量： extern const int bufsize;

(3) 对一个 const 变量的引用称为常量引用。常量引用只能赋值给另外一个常量引用，而不能赋值给另外一个非常量引用。一个普通的对象可以赋值给一个常量引用。
允许为一个常量引用绑定非常量的对象、字面值或者表达式。

(4)常量不能转换成非常量，但是非常量可以转换成常量，只有常量才能转换成常量。


5、 constexpr ：用来声明一个常量，用于常量与函数的返回值转换

constexpr int mf = 20;
constexpr int sz = size(); //只有当 size() 是一个 constexpr 函数时，这条语句才正确，否则错误。
如果某个变量一定是一个常量表达式，将它声明为 constexpr 类型。

(1) 如果某个指针被声明为 constexpr ，则 constexpr 只对该指针有效，对它指向的对象无关。
const int *p = nullptr;		//p是一个指向整型常量的指针
constexpr int *q = nullptr;	//q是一个指向整型的常量指针


6、类型别名
(1) typedef double wages; //wages 是 double 的别名
(2) using SI = Sales_item; // SI 是 Sales_item 的别名


7、 auto
(1) 定义为 auto 的变量，由编译器来推断该变量的类型
auto i = 0, pi=3.14;

(2)
auto &h =42; //错误：不能为非常量引用绑定字面值
const auto &j =42; //正确，可以为常量引用绑定字面值


8、 decltype :选择并返回操作数的数据类型

const int ci = 0;
decltype(ci) x = 0; //x的类型是 const  int

(1) decltype((variable)) 双层括号的结果永远都是引用类型。

(2) 如果 decltype 的内容是指针的解引用操作，则 decltype 将得到引用类型
int i = 42, *p = &i, &r = i;
decltype(r+0) b; //正确，加法的结果是 int 类型，因此 b 是一个未初始化的 int 变量
decltype(*p) c;    //错误:c是 int & 类型，必须初始化
decltype(r)  结果是引用类型。


9、 类内初始值：创建一个对象时，类内初始值将用于初始化数据成员，没有初始化的成员将会被默认初始化。

struct Sales_data {
	std::string bookNO;
	unsigned units_sold = 0;
	double revenue = 0.0;
};


10、 预处理器

预处理器是在编译之前执行的一段程序，比如 #include ，当预处理器看到 #include 标记时就会使用指定的头文件内容替换 #include
头文件保护符，可以防止重复包含头文件。
#ifndef XXX
#define XXX
#include "xxx.h"
.....
#endif


11、 命名空间
using namespace name



12、 标准库类型 string

(1) 包含头文件和命名空间
#include <string>
using std::string

(2) 
string s1; //默认初始化 s1 为空字符串
string s2(5,'c'); //s2 的内容是 ccccc
string s3(s2); //s3是s2的副本
string s4("value"); //s4是字面值 "value" 的副本，除了字面值最后的那个 '\0' 字符
string s5 = "value"; //同上

(3)
string s6 = "hey"; //拷贝初始化
string s7("jack"); //直接初始化
有赋值符号 = 的是拷贝初始化

(4)
getline(is,s); //从 is 中读取一行赋值给 s ，返回 is
s.empty(); //s为空返回 true ，否则返回 false
s.size(); //返回 s 中字符的个数

(5)C++11新标准：范围语句
for(declaration:expression)
	statement
其中， expression 是一个对象，用于表示一个序列。
declaration 定义一个变量，该变量用于访问序列中的基础元素。
每次迭代， declaration 变量会被初始化为 expression 的下一个元素值。
例如：
string str("something");
for(auto c : str)
	cout << c << endl;

	
13、 标准库 vector 容器

(1)头文件和命名空间
#include <vector>
using std::vector
vector 是一个类模板

(2)
vector<int> ivec; //ivec保存 int 类型的对象
vector<vector<string>> file; //该向量的元素是 vector 对象

(3)定义一个空 vector ，然后当运行时获取到元素的值后再逐一添加。
vector<string> svec; //默认初始化， svec 不含任何元素

(4)
vector<int> v1(10);		//v1有10个元素，每个的值都是0
vector<int> v2{10};		//v2有1个元素，值为10
vector<int> v3(10,1);		//v3有10个元素，每个的值都是1
vector<int> v4{10, 1};		//v4有2个元素，值为10,1 

vector<string> v5{"hi"};		//列表初始化，v5有一个元素
vector<string> v6("hi");		//错误：不能使用字符串字面值构建 vector 对象
vector<string> v7{10};		//v7有10个默认初始化的元素
vector<string> v8{10, "hi"};	//v8有10个值为"hi"的元素

(5)向 vector 对象中添加元素
使用成员函数 push_back ，把一个值当成 vector 对象的尾元素压到 vector 对象的尾端。
例如： 
vector<int> v2;
v2.push_back(199); //将199压到 v2 的尾端，此时v2中有一个值为199的元素。
如果循环体内部包含向 vector 对象添加元素的语句，则不能使用范围 for 语句。
也就是不能在范围 for 循环中向 vector 对象添加元素，比如 push_back ，会使该 vector 对象的迭代器失效。
范围语句
for(declaration:expression)
	statement

(6)其他方法
v.empty() 		如果v不含有任何元素，返回真；否则返回假；
v.size()			返回v中元素的个数
v.push_back(t)	向v的尾部添加一个值为t的元素

(7)迭代器 iterator
auto b = v.begin(), e = v.end();
由编译器决定b和e的类型，b表示v的第一个元素，e表示v的尾元素的下一位置。
如果v为空，则 begin() 和 end() 返回的都是尾元素的下一位置。

*iter 		返回迭代器 iter 所指元素的引用
iter->mem   	等价于 (*iter).mem
++iter		令 iter 指示容器的下一个元素
--iter  		令 iter 指示容器的上一个元素

例如：
string s("something");
if ( s.begin() != s.end() ) {
	auto it = s.begin();
	*it = toupper(*it); //将当前字符改成大写字符
}
结果： Something	

for ( auto it = s.begin(); it != s.end() && !isspace(*it); ++it  )	
	*it = toupper(*it);
	
迭代器的类型：
vector<int>::iterator it;			//it能读写 vector<int>的元素
string::iterator it2;				//it2能读写 string 类型的字符

vector<int>::const_iterator it3;	//it3只能读 vector<int>的元素，不能写元素
string::const_iterator it4;			//it4只能读 string 类型的字符，不能写字符
	
auto it3 = v.cbegin(); //it3的类型是 vector<int>::const_iterator	
使用 cbegin cend ，无论 vector 对象本身是否是常量，返回值都是 const_iterator 。


14、 数组
unsigned cnt = 42;
constexpr unsigned sz = 42;
int * parr [sz]; //正确
string bad[cnt]; //错误
string strs[get_size()]; //当 get_size 是 constexpr 时正确，否则错误。
const char a[6] = "abcdef"; //错误，没有地方存放 '\0'
int & refs[10]; //错误，数组中的元素不能使引用
int (*parray)[10] = &arr ; 	//parray 指向一个含有10个 int 整数的数组
int (&arrref)[10] = arr; 	//arrref 引用一个含有10个 int 整数的数组

数组也可以使用范围 for 语句：
for(auto i : array)
	cout << i << endl;

int ia[]={1, 2, 3};
int * beg = begin(ia);	//beg指向 ia 的首元素的指针
int * last = end(ia);	//last指向 ia 尾元素的下一位置的指针

strlen(p); //返回p的长度， '\0'不计算在内
sizeof(p); //返回p的长度， '\0'计算在内， sizeof 用于计算某个表达式所占的字节数
strcmp(p1,p2);
strcat(p1,p2);
strcpy(p1,p2);
使用上面几个函数时，传入的指针必须以空字符作为结束的数组；

char ca[] = {'a', 'b', 'c'};
cout << strlen(ca) <<endl; //错误， ca 没有以 '\0'结束, strlen 函数有可能沿着 ca 在内存中的位置不断向前寻找，直到遇到 '\0' 才停下来。

多维数组：
int ia[3][4]; //大小为3的数组，每个元素都是含有4个整数的数组
int arr[10][20][30]; //大小为10的数组，它的每个元素都是大小为20的数组，这些数组的元素是含有30个整数的数组；
int (&raw)[4] = ia[1]; //将 row 绑定到 ia 的第二个4元素数组上

范围语句 for 处理多维数组时，除了最内层的循环外，其他所有循环的控制变量都应该是 auto & 类型。
size_t cnt =0;
for (const auto &row : ia) 		//对外层数组的每一个元素
	for (auto col : row)		//对内层数组的每一个元素
		cout << col <<endl;
使用 auto & 类型是为了避免转换成指针：
for (auto row : ia ) //row的类型是 int *
	for (auto col : row )
	
int ia[3][4];	
int (*p)[4] = ia;	//p指向含有4个整数的数组
p = &ia[2];			//p指向 ia 的尾元素


15、隐式类型转换
常量整数值0或者字面值 nullptr 能转换成任意指针类型。
指向任意非常量的指针都能转换成 void * 。
指向任意对象的指针能转换成 const void * 。


16、 强制类型转换
static_cast ：  用于隐式类型转换
double slope = static_cast<double>(j); //将 j 强制转换成 double 类型

dynamic_cast ：

const_cast ：去掉 const 属性，只能使用于常量 const
const char *pc;
char *p = const_cast<char*>(pc); //以后可以对 p进行赋值操作

const char *cp;
char *q = static_cast<char*>(cp); //错误， static_cast 不能转换 const 类型
static_cast<string>(cp);		  //正确，字符串字面值转换成 string 类型
const_cast<string>(cp);			  //错误， const_cast 只能改变常量属性


reinterpret_cast ：
int * ip;
char * pc = reinterpret_cast<char*>(ip); //必须牢记 pc 所指向的真实对象是一个 int 类型而非 char 类型。
如果把 pc 当成了普通的字符指针使用


16. 运算符优先级： C++ primer P173(p147)
1）. 左结合性：
:: 全局作用域/类作用域/命名空间作用域。如：  ::name		class::name	  namespace::name
.  成员选择。如：object.member
-> 成员选择。如：point->member
[] 下标。如： a[5]
() 函数调用/类型构造。如： function(param)    type(expr_list)
算术运算符：* / % + - 
移位运算符： << >>
关系运算符： > < >= <= == !=
位与/位异或/位或： & ^ \ 
逻辑与/逻辑或： && ||
逗号：，


2）. 右结合性：
++/--：自增和自减
类型ID/运行时类型ID： typeid(type)    typeid(expr)
类型转换： static_cast<double>(j)     (type)expr
~：位求反
!: 逻辑非
-/+： 一元负号/一元正号
*/&: 解引用/取地址
sizeof: 对象/类型/参数大小
new/new[]/delete/delete[] ：创建对象/创建数组/释放对象/释放数组
noexcept: 能否抛出异常，noexcepr(expr)
指向成员选择的指针： ->*  .*  如：ptr->*member, obj.*member
条件运算符： ?:
赋值： = += -+ *= %= <<= >>= &= |= ^=
throw: 抛出异常




第5章、语句

1、语句作用域
while( int i = get_num() )
	cout << i << endl;
i = 0; //错误：在循环体外部不能访问 i


2、if语句
if (condition)
	statement
else if (condition)
	statement
else
	statement


3. break语句
break负责终止离他最近的while、do while、for或者switch语句，并从这些语句之后的第一条语句开始继续执行。	


4、 continue语句
终止最近的循环中的当前迭代并立即开始下一次迭代。它只能出现在for、while和do while循环的内部。
	
	
5、 异常处理
(1) throw
(2)	try catch
try {
	statement
} catch(runtime_error err){
	statement
}	




第6章 函数
1.
对于程序，编译器会对其分配一段内存，在逻辑上可以分为代码段，数据段，bss段，堆，栈
代码段：保存程序文本，指令指针EIP就是指向代码段，可读可执行不可写
数据段：保存初始化的全局变量和静态变量，可读可写不可执行
BSS：未初始化的全局变量和静态变量
堆(Heap)：动态分配内存，向地址增大的方向增长，可读可写可执行
栈(Stack)：存放局部变量，函数参数，当前状态，函数调用信息等，向地址减小的方向增长，可读可写可执行	

2、函数的返回值
1）函数返回局部对象的引用或者局部对象的指针都是错误的。但是可以返回局部变量。	
2）列表初始化返回值： return { };

3、
声明内联函数： inline
声明常量函数： constexpr


4、 assert
assert是一种预处理宏， assert(expr)
expr必须为真。
如果为假，assert输出错误信息并且终止程序运行。
如果为真，程序继续往下运行。

编译器定义了一个局部静态变量存放函数名字：__func__
预处理器也定义了4个名字：
__FILE__  存放文件名的字符串字面值
__LINE__  存放当前行号的整型字面值 
__TIME__  存放文件编译时间的字符串字面值
__DATE__  存放文件编译日期的字符串字面值




第7章 类

1、this表示类指针，在类的成员函数中，通过它可以访问类的成员： this->member

2、构造函数
1）默认构造函数： Sales_data() = default;
如果=default在类的内部，则默认构造函数是内联的。
如果=default在类的外部，则该构造函数不是内联的。

3、友元
1）类可以允许其他类或者其他函数访问它的非公有成员，方法是令其他类或者函数成为它的友元。
如果类想把一个函数作为它的友元，只需要增加一条以friend关键字开始的函数声明语句即可。
友元声明只能出现在类定义的内部。
友元不是类的成员也不受类所在区域访问控制级别的约束。
友元类的成员函数可以访问此类的所有成员，包括private成员。


4、 C++中 const，mutable，volatile的区别

1）const
const修饰变量：表示该变量的值在初始化之后，就不能再改变它。
const修饰函数实参，表示该函数不能修改实参的值。
const修饰类对象/类对象指针/类对象引用，该const对象只能调用它的const成员函数，不能调用它的非const成员函数。
不能在类定义中定义一个const数据成员。如果要在类定义中用到const成员，将它定义为enum枚举量。
const修饰类成员函数，表示它不能改变对象的成员变量，也不能调用类中的任何非const成员函数。

const常量与define宏定义的区别：
编译器处理方式不同：define宏是在预处理阶段展开。const常量是编译运行阶段使用。
类型和安全检查不同：define宏没有类型，不做任何类型检查，仅仅是展开。const常量有具体的类型，在编译阶段会执行类型检查。
存储方式不同：define宏仅仅是展开，有多少地方使用，就展开多少次，不会分配内存。const常量会在内存中分配(可以是堆中也可以是栈中)。


2）volatile
用它声明的类型变量表示可以被某些编译器未知的因素更改，比如操作系统、硬件或者其它线程等。遇到这个关键字声明的变量，
编译器对访问该变量的代码就不再进行优化，从而可以提供对特殊地址的稳定访问。
当要求使用volatile 声明的变量的值的时候，系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据，而且读取的数据立刻被寄存。
例：
volatile int i=10;
int a = i;
。。。//其他代码，并未明确告诉编译器，对i进行过操作
int b = i;

volatile 指出 i是随时可能发生变化的，每次使用它的时候必须从i的地址中读取，因而编译器生成的汇编代码会重新从i的地址读取数据放在b中。
而优化做法是，由于编译器发现两次从i读数据的代码之间的代码没有对i进行过操作，它会自动把上次读的数据放在b中。而不是重新从i里面读。
这样以来，如果i是一个寄存器变量或者表示一个端口数据就容易出错，所以说volatile可以保证对特殊地址的稳定访问，例如寄存器地址。

下面是volatile变量的几个例子：
并行设备的硬件寄存器（如：状态寄存器）
一个中断服务子程序中会访问到的非自动变量(Non-automatic variables)
多线程应用中被几个任务共享的变量

硬件、中断、RTOS，所用这些都要求volatile变量。

问题：
1). 一个参数既可以是const还可以是volatile吗？解释为什么。
2). 一个指针可以是volatile 吗？解释为什么。
3). 下面的函数有什么错误：
int square(volatile int *ptr)
{
  return *ptr * *ptr;
}

答案：
1). 是的。一个例子是只读的状态寄存器。它是volatile因为它可能被意想不到地改变。它是const因为程序不应该试图去修改它。
2). 是的。尽管这并不很常见。一个例子是当一个中服务子程序修该一个指向一个buffer的指针时。
3). 这段代码的目的是用来返回指针*ptr指向值的平方，但是，由于*ptr指向一个volatile型参数，编译器将产生类似下面的代码：

int square(volatile int *ptr)
{
	int a,b;
	a = *ptr;
	b = *ptr;
	return a * b;
}

由于*ptr的值可能被意想不到地该变，因此a和b可能是不同的。结果，这段代码可能返不是你所期望的平方值！正确的代码如下：
long square(volatile int *ptr)
{
	int a;
	a = *ptr;
	return a * a;
}


3）mutable
mutable是为了突破const的限制而设置的。被mutable修饰的变量(mutable只能用于修饰类的非静态数据成员)，将永远处于可变的状态，即使在一个const函数中。
mutable修饰的变量在被const修饰的成员函数里面也能被修改。


5、 静态成员
静态成员属于类，而不属于对象。
1）static 成员函数
静态成员函数不与任何对象绑定在一起，它们不包含this指针。
静态成员函数不能声明成const。



/***********************************************************************************************
******   第II部分：C++标准库
************************************************************************************************/
第8章 IO库
end。

第9章 顺序容器
end。

第10章 泛型算法
continue。

第11章 关联容器

第12章 动态内存



/***********************************************************************************************
******   第III部分：类设计者的工具
************************************************************************************************/
第13章 拷贝控制

第14章 重载运算与类型转换

第15章 面向对象程序设计

第16章 模板和泛型编程



/***********************************************************************************************
******   第 IV部分：高级主题
************************************************************************************************/

第17章 标准库特殊设计

第18章 用于大型程序的工具

第19章 特殊工具与技术



/***********************************************************************************************
*
************************************************************************************************/
```

