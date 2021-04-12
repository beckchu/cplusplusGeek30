#pragma once

#include<functional>
#include<algorithm>

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
			!shared_count_->reduce_count()) {
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

	template<typename T>
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
	auto operator<<(std::ostream & os, const T & container)->decltype(container.begin(), container.end(), os) {
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
auto compose(_Fn&& f, _Fargs&& ... args) {
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
	return f(get<I>(forward<Tuple>(t))...);
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
