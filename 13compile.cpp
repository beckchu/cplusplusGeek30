#include<iostream>
#include<vector>

using namespace std;

template <bool>
struct compile_time_error;//声明，
template <>
struct compile_time_error<true> {};//特化，定义

#define STATIC_ASSERT(Expr, Msg)   \
  {                                \
    compile_time_error<bool(Expr)> \
      ERROR_##_Msg;                \
    (void)ERROR_##_Msg;            \
  }

//n！
template <int n>
struct factorial {
	static_assert(n >= 0, "Arg must be non-negative");
	static const int value =
		n * factorial<n - 1>::value;
};

template <>
struct factorial<0> {
	static const int value = 1;
};

//if else
template <bool cond,
	typename Then,
	typename Else>
	struct If;

template <typename Then,
	typename Else>
	struct If<true, Then, Else> {
	typedef Then type;
};

template <typename Then,
	typename Else>
	struct If<false, Then, Else> {
	typedef Else type;
};

//while
template <bool condition,
	typename Body>
	struct WhileLoop;

template <typename Body>
struct WhileLoop<true, Body> {
	typedef typename WhileLoop<Body::cond_value, typename Body::next_type>::type type;
};

template <typename Body>
struct WhileLoop<false, Body> {
	typedef typename Body::res_type type;
};

template <typename Body>
struct While {
	typedef typename WhileLoop<Body::cond_value, Body>::type type;
};

template <class T, T v>
struct Integral_constant {//integral_constant是库类型；
	static const T value = v;
	typedef T value_type;
	typedef Integral_constant type;
};

template <int result, int n>
struct SumLoop {
	static const bool cond_value = n != 0;
	static const int res_value = result;
	typedef Integral_constant<int, res_value> res_type;
	typedef SumLoop<result + n, n - 1> next_type;//最终的推导是Intergral_constant<int,10+9+8...0>
};

template <int n>
struct Sum {
	typedef SumLoop<0, n> type;
};

//class特化
typedef std::integral_constant<bool, true> true_type;
typedef std::integral_constant<bool, false> false_type;

template <typename T>
class SomeContainer {
public:
	…
		static void destroy(T* ptr)
	{
		_destroy(ptr,is_trivially_destructible<T>());
	}

private:
	//下面是函数重载，true_type and false_type  是class，_destory(T* ptr,true_type a)
	static void _destroy(T* ptr,true_type)
	{}
	static void _destroy(T* ptr, false_type)
	{
		ptr->~T();
	}
};

//class模板特化 const char*是指向const char的指针，而不是指向char的const指针；char* const才可以remove
//template <class T>
//struct remove_const {//string& --> string&
//	typedef T type;
//};
//template <class T>
//struct remove_const<const T> {//const string&-->string&
//	typedef T type;
//};

//_v是编译期常量，
//template <class T> inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;
//_t是类型别名，
//template <class T> using remove_const_t = typename remove_const<T>::type;

//函数式编程中的map
template <template <typename, typename> class OutContainer = vector, typename F, class R>
auto fmap(F&& f, R&& inputs)
{
	typedef decay_t<decltype(f(*inputs.begin()))> result_type;
	OutContainer<result_type, allocator<result_type>> result;
	for (auto&& item : inputs) {
		result.push_back(f(item));
	}
	return result;
}


vector<int> v{ 1, 2, 3, 4, 5 };
int add_1(int x)
{
	return x + 1;
}

int main() {
	STATIC_ASSERT(true, bad);
	printf("%d\n", factorial<10>::value);
	printf("%d\n", While<Sum<10>::type>::type::value);
	auto result = fmap(add_1, v);
	for (auto r : result) {
		printf("%d\n", r);
	}
}

