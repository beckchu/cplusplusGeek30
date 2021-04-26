#pragma once

#include<functional>
#include<algorithm>
#include<condition_variable>
#include<mutex>
#include<atomic>
#include<set>
#include<future>


using namespace std;
class shared_count {
public:
	shared_count() noexcept
		: count_(1) {}
	void add_count() noexcept
	{
		++count_;
	}
	long reduce_count() noexcept
	{
		return --count_;
	}
	long get_count() const noexcept
	{
		return count_;
	}

private:
	long count_;
};

template<typename T>
class smart_ptr {
public:
	template<typename U> friend class smart_ptr;
	explicit smart_ptr(T* ptr = nullptr)
		: ptr_(ptr)
	{
		if (ptr) {
			shared_count_ =
				new shared_count();
		}
	}

	~smart_ptr() {
		if (ptr_ &&
			!shared_count_->reduce_count()) {//这里要判空的，因为移动后
			delete ptr_;
			delete shared_count_;
		}
	}

	smart_ptr(const smart_ptr& other) {
		ptr_ = other.ptr_;
		if (ptr_) {
			other.shared_count_->add_count();
			shared_count_ = other.shared_count_;
		}
	}

	template<typename U>
	smart_ptr(const smart_ptr<U>& other) noexcept {
		ptr_ = other.ptr_;
		if (ptr_) {
			other.shared_count_->add_count();//封装性是以类来说的；类内同一类型的对象可以访问私有成员；
			shared_count_ = other.shared_count_;
		}
	}

	template<typename U>
	smart_ptr(smart_ptr<U>&& other) noexcept {//被移动的对象，会被改变，析构函数要判空的；
		ptr_ = other.ptr_;
		if (ptr_) {
			other.ptr_ = nullptr;
			shared_count_ = other.shared_count_;
		}
	}

	smart_ptr&
		operator=(smart_ptr rhs) noexcept {//左右值都可传进来，拷贝构造函数和移动构造函数
		rhs.swap(*this);
		return *this;
	}

	void swap(smart_ptr& other) noexcept {
		using std::swap;
		swap(other.ptr_, ptr_);//在类中和类同一类型的对象可以访问私有成员
		swap(other.shared_count_, shared_count_);
	}

	T& operator*() const noexcept {
		return *ptr_;
	}

	T* operator->() const noexcept {
		return ptr_;
	}

	operator bool() const noexcept {
		return ptr_;
	}

	int use_count() {
		return shared_count_->get_count();
	}

private:
	T* ptr_;
	shared_count* shared_count_;
};


template<typename T>
void swap(smart_ptr<T> lhs, smart_ptr<T> rhs) {
	lhs.swap(rhs);//这样写的目的是为了访问类的私有成员
}

//bool()实现类型与bool的隐式转换,类的偏特化实现，声明时刻参考vector<int> a;
template<typename T1, typename T2>
class is_same_type {
	operator bool() {
		return false;
	}
};

template<typename T>
class is_same_type<T, T> {
	operator bool() {
		return true;
	}
};

namespace pushEmplace {
	//这里的描述可以很好地解释item42的三点
	//emplace_back() vs push_back()的区别
	//非基于节点的标准库容器（vector,deque,string），已经提早分配好了内存空间，
	//vector<string> v;
	//v[0] = string("");//会调用对应的赋值构造函数，赋值构造函数的参数是string，非string类型要生成一个临时变量string
	template<typename T>
	void push_back(T&&) {//通用引用，左右值都可以作为参数，但是类型是T，非T类型要调用构造函数生成临时变量

	}

	template<typename ...ARG>
	void emplace_back(ARG ...arg) {

	}
}

template<typename T>
struct less :public binary_function<T, T, bool> {//struct默认是public继承,class默认是private继承
	bool operator()(const T& x, const T& y) const {
		return	x < y;
	}
};

//template<class T> struct hash;

//template<>
//struct hash<int> :public unary_function<int, size_t> {
//	size_t operator()(int v) const noexcept {
//		return static_cast<size_t>(v);
//	}
//};

//[first,last)下面两个函数遵循前闭后开的原则；
//lower_bound
//upper_bound

class istream_line_reader {
public:
	class iterator {
	public:
		typedef ptrdiff_t difference_type;
		typedef string value_type;
		typedef const value_type* pointer;
		typedef const value_type& reference;
		typedef input_iterator_tag iterator_category;

		iterator() noexcept :stream_(nullptr) {}
		explicit iterator(istream& is) :stream_(&is) {
			++* this;//调用的是自己的++运算符
		}

		reference operator*() const noexcept {
			return line_;
		}

		pointer operator->() const noexcept {
			return &line_;
		}

		iterator& operator++() {
			getline(*stream_, line_);
			if (!*stream_) {//和end()对应
				stream_ = nullptr;
			}
			return *this;
		}
		//这里可以看出for循环中为什么建议使用++i，而不是用i++
		iterator operator++(int) {
			iterator temp(*this);
			++* this;
			return temp;
		}
		//这个用来做对象的判断
		bool operator==(const iterator& rhs) const noexcept {
			return stream_ == rhs.stream_;
		}

		bool operator!=(const iterator& rhs) const noexcept {
			return !operator==(rhs);
		}
	private:
		istream* stream_;
		string line_;
	};

	istream_line_reader() noexcept : stream_(nullptr) {}

	istream_line_reader(istream& is) noexcept :stream_(&is) {}

	iterator begin() {
		return iterator(*stream_);
	}

	iterator end() {
		return iterator();
	}

private:
	istream* stream_;
};

//泛型编程
template<bool>
struct compile_time_error;//声明

template<> struct compile_time_error<true> {};//定义

#define STATIC_ASSERT(Msg,Expr)\
{\
	compile_time_error<bool(Expr)>\
		ERROR_##_Msg;\
}\

//模板元编程
template<int n> struct factorial {
	static const int value = n * factorial<n - 1>::value;
};

template<>
struct factorial<0> {
	static const int value = 1;
};

template<bool cond,
	typename Then,
	typename Else>
	struct If;

//模板特化
template<typename Then,
	typename Else>
	struct If<true, Then, Else> {
	typedef Then type;
};

template<typename Then,
	typename Else>
	struct If<false, Then, Else> {
	typedef Else type;
};

template<bool condition, typename Body>
struct whileLoop;

template<typename Body>
struct whileLoop<true, Body> {
	typedef typename whileLoop<Body::cond_value,
		typename Body::next_type>::type type;
};

template<typename Body>
struct whileLoop<false, Body> {
	typedef typename Body::res_type type;
};

template<typename Body>
struct While {
	typedef typename whileLoop<Body::cond_value, Body>::type type;
};
//函数用重载，对象用模板
namespace stdstd {
	//标准库中就有，实现相同
	template<class T, T v>
	struct integral_constant {
		static const T value = v;//模板元编程很喜欢用static const 
		typedef T value_type;
		typedef integral_constant type;
	};

	template<int result, int n>
	struct sumLoop {
		static const bool cond_value = n != 0;
		static const int res_value = result;
		typedef integral_constant<int, res_value> res_type;
		typedef sumLoop<result + n, n - 1> next_type;
	};

	template<int n>
	struct Sum {
		typedef sumLoop<0, n> type;
	};

	typedef std::integral_constant<bool, true> true_type;
	typedef std::integral_constant<bool, false> false_type;
}

namespace RemoveConst {
	template<class T>
	struct remove_const {
		typedef T type;
	};

	namespace removeConst {
		template<class T>
		struct remove_const<const T> {
			typedef T type;
		};

		template<class T>
		using remove_const_t=typename remove_const<T>::type;
	}
}


//内联变量since c++17
//template<class T>
//inline constexpr bool
//is_trivially_destructible_v = is_trivially_destructible<T>::value;

//模板其实对传入的参数要求挺高的；
template<template<typename, typename>class OutContainer = vector, 
		typename F, class R>
	auto fmap(F&& f, R&& inputs) {
	typedef decay_t<decltype(f(*inputs.begin()))> result_type;//decay_t返回值语义

	OutContainer<result_type, allocator<result_type>> result;

	for (auto&& item : inputs) {//auto&&通用引用
		result.push_back(f(item));
	}
	return result;
}


//判断类中是否含有对应的函数；这个方法比较复杂，下面有更简单的
template<typename T>
struct has_reserve {
	struct good { char dummy; };
	struct bad { char dummy[2]; };
	template<class U, void (U::*)(size_t)> struct SFINAE {};
	template<class U> static good reserve(SFINAE<U, &U::reserve>*);
	template<class U> static bad reserve(...);
	static const bool value = sizeof(reserve<T>(nullptr)) == sizeof(good);//一个只有声明的函数的大小，为其返回值的大小，static一样；
};

//直接查enable_if的 说明手册
namespace EnableIF {
	template<typename C, typename T>
	enable_if_t<has_reserve<C>::value, void>
		append(C& container, T* ptr, size_t size) {
		container.reserve(container.size() + size);
		for (size_t i = 0; i < size; i++) {
			container.push_back(ptr[i]);
		}
	}

	template<typename C, typename T>
	enable_if_t<!has_reserve<C>::value, void>
		append(C& container, T* ptr, size_t size) {
		container.reserve(container.size() + size);
		for (size_t i = 0; i < size; i++) {
			container.push_back(ptr[i]);
		}
	}
}


namespace Declval {
	template<typename C, typename T>//c++中的逗号表达式的意思是按顺序逐个估值，并返回最后一项
	auto append(C& container, T* ptr, size_t size)->decltype(declval<C&>().reserve(1U), void()) {
		container.reserve(container.size() + size);
		for (size_t i = 0; i < size; i++) {
			container.push_back(ptr[i]);
		}
	}
}

namespace voidT {
	template<typename...>//这里才是最简单的实现
	using void_t=void;

	template<typename T, typename = void_t<>>
	struct has_reserve :false_type {};

	template<typename T>//返回的是void
	struct has_reserve<T, void_t<decltype(declval<T&>().reserve(1U))>> :true_type {};//decalval直接查看定义
}


//标签分发
namespace LabelDistribution {
	template<typename C, typename T>
	void _append(C& container, T* ptr, size_t size, true_type) {
		container.reserve(container.size() + size);
		for (size_t i = 0; i < size; i++) {
			container.push_back(ptr[i]);
		}
	}

	template<typename C, typename T>
	void _append(C& container, T* ptr, size_t size, false_type) {
		container.reserve(container.size() + size);
		for (size_t i = 0; i < size; i++) {
			container.push_back(ptr[i]);
		}
	}

	template<typename C, typename T>
	void append(C& container, T* ptr, size_t size) {
		_append(container, ptr, size, integral_constant<bool, has_reserve<C>::value>{});
		//_append(container, ptr, size, has_reserve<C>{});
	}
}

namespace OutPutContainer {
	template<typename T>
	struct is_pair :std::false_type {};

	template<typename T, typename U>
	struct is_pair<std::pair<T, U>> :std::true_type {};//直接看文档的实现，和上面的实现是一样的；

	template<typename T>
	constexpr bool is_pair_v = is_pair<T>::value;//理解模板template<typename T>中T的意思；

	template<typename T>
	struct has_output_function {
		template<class U>
		static auto output(U* ptr)->decltype(std::declval<std::ostream&>() << *ptr, std::true_type);//declval声明一个临时变量
		template<class U>
		static std::false_type output(...);
		static constexpr bool value = decltype(output<T>(nullptr))::value;
	};

	template<typename T>
	constexpr bool has_output_function_v = has_output_function<T>::value;
	//下面已经有定义了，这里应该不需要了
	//template<typename T, typename U>
	//std::ostream& operator<< (ostream& os, pair< T, U >& pr);

	//template<typename T, typename Cont>
	//auto output_element(ostream& os, const T& element, const Cont&, const std::true_type)
	//	->decltype(std::declval<typename Cont::key_type>(), os);

	//template<typename T, typename Cont>
	//auto output_element(std::ostream& os, const T& element, const Cont&, ...)->decltype(os);

	template<typename T, typename = std::enable_if_t<!has_output_function_v<T>>>
	auto operator<<(std::ostream & os, const T & container)->decltype(container.begin(), container.end(), os) {//decltype 也算是模板推导
		using std::decay_t;
		using std::is_same_v;

		using element_type=decay_t<decltype(*container.begin())>;
		constexpr bool is_char_v = is_same_v<element_type, char>();
		if constexpr (!is_char_v) {
			os << "{";
		}
		if (!container.empty()) {
			auto end = container.end();
			bool on_first_element = true;
			for (auto it = container.begin(); it != end; ++it) {
				if constexpr (is_char_v) {//编译器确定丢掉那个语句
					if (*it == '\0') {
						break;
					}
				}

				if constexpr (!is_char_v) {
					if (!on_first_element) {
						os << ", ";
					}
					else {
						on_first_element = false;
					}
				}
				output_element(os, *it, container, is_pair<element_type>());
			}
		}
		if constexpr (!is_char_v) {
			os << " }";
		}
		return os;
	}

	template<typename T,typename Cont>
	auto output_element(std::ostream& os, const T& element, const Cont&, const std::true_type)
		->decltype(std::declval<typename Cont::key_type>(), os) {//关联容器有key_type成员变量
		os << element.first << "==>" << element.second;
		return os;
	}

	template<typename T, typename Cont>
	auto output_element(std::ostream& os, const T& element, const Cont&, ...)
		->decltype(os) {
		os << element;
		return os;
	}

	template<typename T,typename U>
	std::ostream& operator<<(std::ostream& os, const std::pair<T, U>& pr) {//pair才这样输出
		os << '{' << pr.first << ", " << pr.second << '}';
		return os;
	}
}

//函数对象
struct adder {
	adder(int n):n_(n){}
	int operator()(int x) {
		return x + n_;
	}
private:
	int n_;
};

int get_count()
{
	static int count = 0;
	return ++count;
}
//这个要找一个更高版本的编译器试一下
class task {
public:
	task(int data) :data_(data) {}
	auto lazy_launch() {
		return
			[=, count = get_count()]() mutable{
			ostringstream oss;
			oss << "Done work " << data_
				<< "<No. " << count
				<< ") int thread "
				<< this_thread::get_id()
				<< '\n';
			msg_ = oss.str();
			calculate();
		};
	}

	void calculate() {
		this_thread::sleep_for(100ms);
		cout << msg_;
	}
private:
	int data_;
	string msg_;
};

//函数式编程，
//高阶函数头文件 https://en.cppreference.com/w/cpp/header/algorithm
//map  transform
//reduce   accumulate
//Filter  copy_if remove_if
//for_each 

//可变模板用法
//转发
template<typename T,typename ... Args>
inline unique_ptr<T>
make_unique(Args&&... args) {
	return unique_ptr<T>(new T(forward<Args>(args)...));
}

//make_unique<vector<int>>(100, 1));
//template<>
//inline unique_ptr<vector<int>>
//make_unique(int&& arg1, int&& arg2) {
//	return unique_ptr<vector<int>>(new vector<int>(forward<int>(arg1), forward<int>(arg2)));
//}

//递归
template<typename T>
constexpr auto sum(T x) {
	return x;
}

template<typename T1,typename T2,typename... Targ>
constexpr auto sum(T1 x, T2 y, Targ... args) {
	return sum(x + y, args...);
}
//这个有什么意义？加法的0，乘法的1，把参数原封不动的返回，感觉没啥意义
inline auto compose() {
	return [](auto&& x)->decltype(auto) {
		return std::forward<decltype(x)>(x);
	};
}

template<typename _Fn>
auto compose(_Fn&& f) {
	return [f = std::forward<_Fn>(f)](auto&& ... x)->decltype(auto){
		return f(std::forward<decltype(x)>(x)...);
	};
}

template<typename _Fn,typename... _Fargs>
auto compose(_Fn&& f, _Fargs&& ... args) {//其实都是函数对象
	return[f = std::forward<_Fn>(f), args...](auto&&... x)->decltype(auto){
		return f(compose(std::move(args)...)(std::forward<decltype(x)>(x)...));
	};
}

//可变参数模板参数包展开
namespace integerSequence{
template <class T, T... Ints>
struct integer_sequence {};

template <size_t... Ints>
using index_sequence = integer_sequence<size_t, Ints...>;

template <size_t N, size_t... Ints>
struct index_sequence_helper {
	typedef	typename index_sequence_helper<N - 1, N - 1, Ints...>::type	type;
};

template <size_t... Ints>
struct index_sequence_helper<0, Ints...> {
	typedef index_sequence<Ints...>	type;
};

template <size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;
//直接手动展开,写一下就出来了
//integer_sequence<size_t, 0, 1, 2, …, N - 1>;

template <class F, class Tuple, size_t... I>
	constexpr decltype(auto)
	apply_impl(F&& f, Tuple&& t,index_sequence<I...>)
{
	return f(get<I>(forward<Tuple>(t))...);//对f有要求的，这里只是展开了，看integer_sequence的example
}
//这样才算懂了tuple，标准库中也有std::integer_sequence
template <class F, class Tuple>
constexpr decltype(auto)
apply(F&& f, Tuple&& t)
{
	return apply_impl(forward<F>(f),forward<Tuple>(t),
		make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}
}

class WfristRWLock {
public:
	WfristRWLock() = default;
	~WfristRWLock() = default;

	void lock_read(){
		std::unique_lock<mutex> ulk(counter_mutex);
		cond_r.wait(ulk, [=]() {return write_cnt == 0; });//这里可以看出来是写优先
		read_cnt++;
	}
	void lock_write(){
		std::unique_lock<mutex> ulk(counter_mutex);
		++write_cnt;//这里为了读优先
		cond_w.wait(ulk, [=]() {return read_cnt == 0 && !inwriteflag; });//并发写也是要处理的
		inwriteflag = true;
	}

	void release_read(){
		std::unique_lock<mutex> ulk(counter_mutex);
		if (--read_cnt == 0 && write_cnt > 0) {//这里解决读取量很大，写入量很小的场景，避免读写线程切换
			cond_w.notify_one();
		}

	}
	void release_write(){
		std::unique_lock<mutex> ulk(counter_mutex);
		if (--write_cnt == 0) {
			cond_r.notify_all();
		}
		else {
			cond_w.notify_one();
		}
		inwriteflag = false;
	}
private:
	atomic<size_t> read_cnt{ 0 };
	atomic<size_t> write_cnt{ 0 };
	atomic<bool> inwriteflag{ false };
	mutex counter_mutex;
	condition_variable cond_w;
	condition_variable cond_r;
};

class Noncopyable {
public:
	Noncopyable() = default;
	~Noncopyable() = default;

private:
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
};

template<typename __RWLockable>
class unique_writeguard:public Noncopyable {
public:
	unique_writeguard(__RWLockable& rw_lockable) :rw_lockable_(rw_lockable) {
		rw_lockable_.lock_write();
	}

	~unique_writeguard() {
		rw_lockable_.release_write();
	}

private:
	__RWLockable& rw_lockable_;
};

template<typename __RWLockable>
class unique_readguard :public Noncopyable {//为什么std::nocopyable找不到
public:
	unique_readguard(__RWLockable& rw_lockable) :rw_lockable_(rw_lockable) {
		rw_lockable_.lock_read();
	}

	~unique_readguard() {
		rw_lockable_.release_read();
	}

private:
	__RWLockable& rw_lockable_;
};

class Customer {
public:
	Customer(const Customer& rhs):name(rhs.name) {
		
	}
	Customer& operator=(const Customer& rhs) {
		name = rhs.name;
		return *this;
	}
private:
	std::string name;
};

class PriorityCustomer :public Customer {
public://子类转化为父类，值语义的话会产生对象切割，这里调用的是customer拷贝构造函数
	//默认的5个函数不能互相实现
	PriorityCustomer(const PriorityCustomer& rhs):Customer(rhs),priority(rhs.priority) {

	}

	PriorityCustomer& operator=(const PriorityCustomer& rhs) {
		Customer::operator=(rhs);//直接进行对象切割，我一般不会这样写
		priority = rhs.priority;
		return *this;
	}

private:
	int priority;
};

class Rational {
public:
	Rational(int numerator = 0, int denominator = 1);//explicit 增加上看一下效果
	const Rational operator* (const Rational& rhs) const;
	int numerator() const;
	int denominator() const;
private:
	int num;
	int den;
};

Rational oneHalf(1, 2);
//Rational result = oneHalf * 2;//只要作为参数列表中才会产生隐式类型转换
const Rational operator*(const Rational& lhs, const Rational& rhs) {
	return Rational(lhs.numerator() * rhs.denominator(),
		lhs.denominator() * rhs.denominator());
}
Rational result1 = 2*oneHalf;

//如果程序取了某个inline函数的地址，这个inline函数将被outline

class Base {
private:
	int x;
public:
	virtual void mf1() = 0;
	virtual void mf1(int);
	virtual void mf2();
	void mf3();
	void mf3(double);
};

class Derived :public Base {
public:
	using Base::mf1;//mf2(int)
	using Base::mf3;//mf3(double)
	virtual void mf1();//继承关系下函数是按照名字覆盖的；
	void mf3();
	void mf4();
};
//继承可以使用，接口组合和委托来替代

//
class Shape {
public:
	enum class ShapeColor{Red,Green,Bule};
	virtual void draw(ShapeColor color = ShapeColor::Red) const = 0;//virtual是多态的，参数默认值时静态的
};

class Rectangle :public Shape {
public:
	virtual void draw(ShapeColor color = ShapeColor::Green) const;
};

class Circle :public Shape {
public:
	virtual void draw(ShapeColor color) const;//对象和指针调用的现象不一样的；
};

//template<typename Container,typename Index>
//auto authAndAccess(Container& c, Index i)->decltype(c[i]) {//这里使用vector时直接返回的引用
//	return c[i];
//}

template<typename Container, typename Index>
decltype(auto) authAndAccess(Container& c, Index i) {//这里使用vector时直接返回的引用,auto推导和模板推导几乎一样
	return c[i];//expr
}


template<typename T> void fT(T& param);

template<class T,size_t N>
void swap(T(&a)[N], T(&b)[N]) noexcept(noexcept(swap(*a, *b)));

template<class T1,class T2>
struct pair {
	void swap(pair& p) noexcept(noexcept(swap(first, p.first)) && noexcept(swap(first, p.first)));
};


class Investment{};
class sock:public Investment{};
class Bond:public Investment{};
class RealEstate:public Investment{};

auto delInvmt = [](Investment* pInvestment) {delete pInvestment; };
class Stock;
template<typename... Ts>
std::unique_ptr<Investment,decltype(delInvmt)>
makeInvestment(Ts&&... params) {
	std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);
	if (true) {
		pInv.reset(new Stock(std::forward<Ts>(params)...));
	}
	return pInv;//unique_ptr没有拷贝构造函数，直接调用move,这里没有返回值优化，RVO
}

//template <class U, class D> shared_ptr(unique_ptr<U, D>&& x);看文档，unique_ptr直接能转换为shared_ptr
//unique_ptr把销毁器看作本身的一部分

template<typename T>
void fff(const T&& param);//因为有const所以并不是通用引用

//class Matrix;
//Matrix operator+(Matrix&& lhs, const Matrix& rhs) {
//	lhs += rhs;
//	return std::move(lhs);//move
//}
//
//Matrix operator+(Matrix&& lhs, const Matrix& rhs) {
//	lhs += rhs;
//	return lhs;//RVO条件,局部变量与返回值类型一样，局部变量就是返回值，不能有if else的选择，参考极客时间
//}

//避免通用引用的重载，应该值得是函数重载

//约束通用引用

//enable_if的两种使用，直接查看其定义
template<class T>
typename std::enable_if<std::is_integral<T>::value,bool>::type
is_odd(T i) { return bool(i % 2); }
//typename=  可选的模板参数，没有名称，且为默认值
template<class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T i) { return !bool(i % 2); }

//modern effective c++ item26的讨论
void log(std::chrono::system_clock::time_point, const string&);
std::multiset<std::string> names;
void logAndAdd(const std::string& name) {
	auto now = std::chrono::system_clock::now();
	log(now, "logAndAdd");
	names.emplace(name);
}

template<typename T>
void logAndAdd(T&& name) {
	auto now = std::chrono::system_clock::now();
	log(now, "logAndAdd");
	names.emplace(std::forward<T>(name));
}

std::string nameFromIdx(int idx);
void logAndAdd(int idx) {
	auto now = std::chrono::system_clock::now();
	log(now, "logAndAdd");
	names.emplace(nameFromIdx(idx));
}


////完美转发构造函数的讨论
//class Person {
//public:
//	template<typename T>
//	explicit Person(T&& n) :name(std::forward<T>(n)){}//forwarding ctor完美转发构造函数
//	explicit Person(int idx):name(nameFromIdx(idx)){}
//
//	Person(const Person& rhs);
//	Person(Person&& rhs);
//
//private:
//	string name;
//};

//约束使用通用引用的模板
//class Person {
//public:
//	template<typename T,
//		typename = typename std::enable_if_t<!is_same<Person, typename std::decay<T>::type>::value>::type>
//		explicit Person(T&& n);
//};

class Person {
public:
	template<typename T,
		typename = std::enable_if_t<!std::is_base_of<Person, std::decay_t<T>>::value &&
		!std::is_integral<std::remove_reference_t<T>>::value>>
		explicit Person(T && n) :name(std::forward<T>(n)) {}

	explicit Person(int idx) :name(nameFromIdx(idx)) {}

private:
	std::string name;
};


class SpecialPerson1 :public Person {
	SpecialPerson1(const SpecialPerson1& rhs):Person(rhs)
	{}//调用base class forwarding ctor
	SpecialPerson1(const SpecialPerson1&& rhs) :Person(std::move(rhs)) 
	{}//calls base class forwarding ctor
};


//移动操作的讨论
//之前对stl容器的认识很错误
//array本质上是具有STL接口的内置数组，没有指针实现，移动操作和指针操作开销基本一样；
//其他容器将内容保存在堆上，本身只保存了堆内存数据的指针；移动数据非常方便
//所以vector的大小，并不包含真实数据的大小；

//string 比较特殊
//SSO，长度小于15的短字符串存储在string缓冲器中，并没有在堆上，移动的操作在某些情况下并不比赋值高

//No move operations
//move not faster
//move not usable 进行移动的上下文要求移动操作不会抛出异常，单这个操作并没有被声明为noexcept

//std::find_if
//std::remove_if
//std::count_if
//std::sort
//std::nth_element
//std::lower_bound

//初始化捕获移动对象到闭包
class Widget{
public:
	bool isValidated() const;
	bool isProcessed() const;
	bool isArchived() const;
private:
	string name;
};
//c++14
auto pw = std::make_unique<Widget>();
auto func = [pw = std::move(pw)](){ return pw->isValidated() && pw->isArchived(); };
auto func1 = [pw = std::make_unique<Widget>()](){ return pw->isValidated() && pw->isArchived(); };

//c++11
class IsValAndArch {
public:
	using DataType=std::unique_ptr<Widget>;
	explicit IsValAndArch(DataType&& ptr):pw(std::move(ptr)){}
	bool operator()()const {
		return pw->isValidated() && pw->isArchived();
	}

private:
	DataType pw;
};

auto func3 = IsValAndArch(std::make_unique<Widget>());

//c++14
std::vector<double> data1;
auto func4 = [data = std::move(data1)]{};
//c++11
auto func5 = std::bind([](const std::vector<double>& data) {}, std::move(data1));

//std::forward的auto&&使用decltype，泛型lambda
//auto f = [](auto x) {return func(normalize(x)); };
//auto f1 = [](auto&& param) {return func(normalize(std::forward<decltype(param)>(param))); };
//auto f2 = [](auto&& ... params) {return func(normalize(std::forward<decltype(param)>(param)...)); };

//基于任务的编程future

//launch policy
//std::launch::async     f必须异步执行
//std::launch::deferred  调用get和wait要求std::async的返回值时f才同步执行，
//						 调用方停止，直到f运行结束，没有调用get，wait，f永远不被执行
//默认 std::launch::async | launch::deferred 允许std::async和标准库的线程管理重建
//						(负责线程的创建或销毁)，避免超载

void fun();
auto fut = std::async(fun);
//无法预测f是否会与t同时运行
//无法预测f是否在调用get和wait的线程（t）上执行，
//无法预测f是否执行

//默认启动策略的调度灵活性导致使用线程本地变量比较麻烦，如果f读取了线程本地变量存储，不能确定那个本地变量被访问

using namespace std::literals;
void ff() {
	std::this_thread::sleep_for(1s);
}

//auto fut1 = std::async(ff);
//while (fut.wait_for(100ms) != std::future_status::ready) {//这样不确定，可能一直执行下去
//
//}

//if (fut.wait_for(0s) == std::future_status::deferred) {
//
//}
//else {
//	while (fut.wait_for(100ms) != std::future_status::ready) {
//
//	}
//}

//真正异步执行的 c++11
template<typename F, typename... Ts>
inline
std::future<typename std::result_of<F(Ts...)>::type>
reallyAsync(F&& f,Ts&&... params)
{
	return std::async(std::launch::async, std::forward<F>(f), std::forward<Ts>(params...));
}
//c++14
//template<typename f,typename... Ts>
//inline
//auto
//reallyAsync(F&& f, Ts&&... params) {
//	return std::async(std::launch::async, std::forward<F>(f), std::forward<Ts>(param...));
//}

//std::thread 和future都视作系统线程句柄
class widget {
public:
	//void addName(const std::string& newName) {
	//	names.push_back(newName);
	//}

	//void addName(std::string&& newName) {
	//	names.push_back(std::move(newName));//对右值引用使用move，对通用引用使用forward
	//}

	//template<typename T>//方法虽好但是提高了代码的复杂度，且必须放在头文件中，会造成过多的实例化函数
	//void addName(T&& newName) {
	//	names.push_back(std::forward<T>(newName));
	//}

	void addName(std::string newName) {//lvalue调用 copy，rvalue调用 move copy，继承关系下不能使用，存在切片问题
		names.push_back(std::move(newName));
	}

private:
	std::vector<std::string> names;
};

class Password {
public:
	explicit Password(std::string pwd):text(std::move(pwd)){}
	void changTo(std::string newPwd) {
		text = std::move(newPwd);//赋值操作，如果stl中内存不够需要搬移，效率和copy的一样
	}
private:
	std::string text;

};
