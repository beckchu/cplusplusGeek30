// testEmplace_back.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include <stdlib.h>
#include<utility>
#include<string>
#include<iterator>
#include<vector>
#include<iostream>
#include<algorithm>
#include<set>
#include<map>
#include<unordered_map>
#include<thread>
#include<array>
#include<sstream>
#include"modernEffectiveAndGeekModernSummy.h"

using namespace std;

class shape {
public:
	virtual ~shape() {}
};

class circle : public shape {
public:
	~circle() { puts("~circle()"); }
};

//自定义字面量
struct length {
public:
	double value;
	enum unit {
		metre,
		kilometre,
		millimetre,
		centimetre,
		inch,
		foot,
		yard,
		mile,
	};
	static constexpr double factors[] =
	{ 1.0, 1000.0, 1e-3,
	  1e-2, 0.0254, 0.3048,
	  0.9144, 1609.344 };

	explicit length(double v, unit u = metre) {
		value = v * factors[u];
	}
};

length operator+(length lhs, length rhs) {
	return length(lhs.value + rhs.value);
}

length operator""_m(long double v) {
	return length(v, length::metre);
}

length operator""_cm(long double v) {
	return length(v, length::centimetre);
}

int add_1(int x) {
	return x + 1;
}

constexpr int sqr(int n) {
	return n * n;
}

constexpr int factorial(int n) {
	//static_assert(n>0,"static error");//编译期，assert运行期，static_assert<constexpr,"">
	if (n == 0) {
		return 1;
	}
	else {
		return n * factorial(n - 1);
	}
}

//tuple
using num_tuple=tuple<int, string, string>;

ostream& operator<<(ostream& os,
	const num_tuple& value) {
	os << get<0>(value) << ','
		<< get<1>(value) << ','
		<< get<2>(value) << ',';
	return os;
}

int main()
{
	/*smart_ptr<circle> ptr1(new circle());
	printf("use count of ptr1 is %ld\n",
		ptr1.use_count());
	smart_ptr<shape> ptr2;
	printf("use count of ptr2 was %ld\n",
		ptr2.use_count());
	ptr2 = ptr1;
	printf("use count of ptr2 is now %ld\n",
		ptr2.use_count());
	if (ptr1) {
		puts("ptr1 is not empty");
	}*/

	is_same_type<char, char>();
	is_same_type<int, char>();
	//右值引用的性能提升，要看他们的实现具体实现

	{
		string name = "eric";
		string result = string("hello, ") + name + ".";

		//emplace_back() vs push_back()的区别
		//两个函数的类似实现在.h文件中，
		vector<string> a;
		a.push_back("xyzzy");//==push_back(string("xyzzy"))
		a.emplace_back("xyzzy");
	}

	{
		vector<int> v{ 13,6,4,11,29 };
		sort(v.begin(), v.end(), greater<int>());
		cout << hex;
		auto hp = hash<int*>();
		cout << "hash(nullptr)  = " << hp(nullptr) << endl;
		cout << "hash(v.data()) = " << hp(v.data()) << endl;
		cout << "v.data()       = " << static_cast<void*>(v.data()) << endl;
		auto hs = hash<string>();
		cout << "hash(\"hello\")  = " << hs(string("hello")) << endl;
		cout << "hash(\"hellp\")  = " << hs(string("hellp")) << endl;
	}

	

	set<int> s{ 1,1,1,2,3,4 };
	multiset<int, greater<int>> mset{ 1,1,1,2,3,4 };//基于红黑树，二差搜索树，本身就是又顺序的；


	multimap<string, int> mmp{
	  {"one", 1},
	  {"two", 2},
	  {"three", 3},
	  {"four", 4}
	};

	mmp.lower_bound("one");
	mmp.upper_bound("two");
	mmp.equal_range("two");
	unordered_map<string, int> unmmp = {{ "one",10 }};
	//unmmp[0];//operator[]()不存在key时会创建一个空的出来

	vector<int> v1{ 1,2,3,4,5 };
	vector<int> v2;
	copy(v1.begin(), v1.end(), back_inserter(v2));//这些都是很经典的用法，直接看函数原型；
	copy(v2.begin(), v2.end(), ostream_iterator<int>(cout, ", "));

	for (const string& line :istream_line_reader(cin)) {
		// 示例循环体中仅进行简单输出
		cout << line << endl;
	}

	int ia = 0;
	decltype(ia) b;
	int ic = 1;
	decltype((ia)) ib = ic;
	decltype(ia + ia) id;
	decltype(auto) ptr=make_pair(1,3);

	multimap<string, int>::iterator lower, upper;
	tie(lower, upper) = mmp.equal_range("four");//cppreference 查看tuple的定义
	
	{
		//列表初始化
		int a[] = { 1,2,3,4,5 };
		vector<int> v{ 1,2,3,4,5 };//构造函数initializer_list参数
		//统一初始化
		class utf8_to_wstring {
		public:
			utf8_to_wstring(const char*);
			operator wchar_t* ();
		};
		//之前不知道这个，要和匿名对象进行区分
		//T1 name1(T2(name2))  此时不把T2(name2)视为[a function style cast],直接视为T2 name2
		ifstream ifs(utf8_to_wstring(filename));
		ifstream ifs(utf8_to_wstring filename);//这里是声明了一个函数
		//ifstream ifs{ utf8_to_wstring{ filename } };
		//{}也会有问题的，列表初始化构造函数和其他构造函数同事存在的话，倾向于列表初始化构造函数
	}
	

	this_thread::sleep_for(500ms);
	{
		1.0_m + 1.0_m;
	}

	//数字分隔符
	unsigned mask = 0b111'000'000;
	long r_earth_equatorial = 6'378'137;

	//泛型编程
	{
		using namespace stdstd;
		While<Sum<10>::type>::type::value;
	}

	{
		vector<int> v{ 1,2,3,4,5 };
		auto result = fmap(add_1, v);
	}

	{
		//constexpr函数可以不产生编译器常量，检验的方法是将其赋给一个constexpr变量
		constexpr int n = sqr(3);
		std::array<int, n> a;
		int b[n];
	}
	

	constexpr int Nn = factorial(10);
	//用法是在编译期使用constexpr
	//运行期使用const

	{
		//函数对象
		adder(3)(10);
		//直接看plus的实现
		auto add_2 = bind2nd(plus<int>(), 2);//bind2nd==bind second param
	}

	{
		//lambda
		//泛型lambda
		auto sum = [](auto x, auto y) {
			return x + y;
		};//好坑啊，把它变成函数了，
		using namespace std::placeholders;
		vector<int> v{ 1,2,3,4,5 };
		transform(v.begin(), v.end(), v.begin(), bind(plus<int>(), _1, 2));

		//std::fuction模板
		map<string, function<int(int, int)>> op_dict{
			{"+",[](int x,int y) {return x + y; }},
			{"-",[](int x,int y) {return x - y; }},
			{"*",[](int x,int y) {return x * y; }},
			{"/",[](int x,int y) {return x / y; }}
		};
	}

	{
		vector<num_tuple> vn{
			{1,"one","un"},
			{2,"two","deux"},
			{3,"three","trois"},
			{4,"four","quatre"}
		};

		get<2>(vn[0]) = "une";
		sort(vn.begin(), vn.end(), [](auto&& x, auto&& y) {return get<2>(x) < get<2>(y); });
		constexpr auto size = tuple_size_v<num_tuple>;
	}

	{
		Derived d;
		int x=2;
		d.mf1();
		d.mf1(x);
		d.mf2();
		d.mf3();
		d.mf3(x);

		Shape* ps;
		Shape* pc = new Circle;//静态类型Shape*，和指针有关系
		Shape* pr = new Rectangle;
		ps = pc;//动态类型Circle
		ps = pr;//动态类型Rectangle

		pc->draw(Shape::ShapeColor::Red);
		pr->draw(Shape::ShapeColor::Red);
		pr->draw();//默认参数调用的是red，很好改的

		vector<int> a = { 1,3,4,5,6,7 };
		authAndAccess(a,2);

	}

	{
		//这里有个地方之前没理解，
		int x = 32;
		const int cx = x;
		const int& rx = cx;
		//template<typename T> void f(T& param);  -->f(T& (const int)) -->f(const int&)
		//f(rx);//expr表达式有引用，先忽略引用部分,剩下的部分再决定T和param类型，之前理解都是错的，可以直接把T&的引用去掉，看一下
	}

	{
		//右值只是移动语义的候选者，std::move()是将对象转换为右值，只是告诉编译器这个对象很适合移动，但是并不保证真的调用移动语义

		//移动语义是要改变原来的对象，参数肯定不是const，当参数是一个const临时变量的话，不会调用移动构造函数，常量的左值可以绑定常量的右值
		//具体调用那个函数，要看cplusplus中实现

		//在调用拷贝构造函数的地方，如果遇到右值会调用移动构造函数，赋值函数也一样；
		//参看cppreference的item23
		
	}

	{
		//这里主要理解emplace的实现，
		//const std::string&
		{std::string petName("Darla");
			logAndAdd(petName);//pass lvalue std::string
			logAndAdd(std::string("persephone"));//pass rvalue std::string，这个没看懂书上的解释
			logAndAdd("Patty Dog");//隐式创建了一个string
		}
		//通用模板调用
		std::string petName("Darla");
		logAndAdd(petName);//copy
		logAndAdd(std::string("persephone"));//move rvalue instead of copying it
		logAndAdd("Patty Dog");//create std::string in multiset instead of copying a temporary std::string

		logAndAdd(22);//调用overload logAndAdd(int idx)

		short nameIdx;
		logAndAdd(nameIdx);//error 直接匹配了通用引用，通用引用可以匹配任何参数（极少数不适合

		//std::string name("Nancy");
		//Person p(name);
		//auto cloneOfp(p);//本应该代用copy构造函数的，这里调用了完美转发构造函数,直接崩掉了

	}

	return 1;

}
