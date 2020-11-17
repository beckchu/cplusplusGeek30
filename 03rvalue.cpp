
#include <iostream>  // std::cout/endl
#include <utility>   // std::move

//编译器发现可以做返回值优化（named return value optimization，或 NRVO）
//c++11编译器无法优化nrvo，就会调用移动构造函数；
using namespace std;

class Obj {
public:
	Obj()
	{
		cout << "Obj()" << endl;
	}
	Obj(const Obj&)
	{
		cout << "Obj(const Obj&)"
			<< endl;
	}
	Obj(Obj&&)
	{
		cout << "Obj(Obj&&)" << endl;
	}
};

Obj simple()
{
	Obj obj;
	// 简单返回对象；一般有 NRVO
	return obj;
}

Obj simple_with_move()
{
	Obj obj;
	// move 会禁止 NRVO
	return std::move(obj);
}

Obj complicated(int n)
{
	Obj obj1;
	Obj obj2;
	// 有分支，一般无 NRVO
	if (n % 2 == 0) {
		return obj1;
	}
	else {
		return obj2;
	}
}

/*int main()
{
	cout << "*** 1 ***" << endl;
	auto obj1 = simple();
	cout << "*** 2 ***" << endl;
	auto obj2 = simple_with_move();
	cout << "*** 3 ***" << endl;
	auto obj3 = complicated(42);
}*/

class shape {

};

void foo(const shape&)
{
	puts("foo(const shape&)");
}

void foo(shape&&)
{
	puts("foo(shape&&)");
}

void bar(const shape& s)
{
	puts("bar(const shape&)");
	foo(s);
}
//模板特化的推导
void bar(shape&& s)
{
	puts("bar(shape&&)");
	foo(s);//传进来的右值会变为左值，
	//利用的引用坍缩
	//foo(std::move(s));//static_cast<shape&&>(s)
	//foo(std::forward<shape&&>(s));
}

template <typename T>
void bar(T&& s)//T&&万能引用
{
	puts("bar(T&& s)");
	foo(std::forward<T>(s));
}

int main()
{
	bar(shape());
}
