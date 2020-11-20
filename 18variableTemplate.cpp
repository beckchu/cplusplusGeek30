#include<vector>
#include<memory>
#include<iostream>
#include<numeric>

using namespace std;

//可变模板
template <typename T, typename... Args>
inline unique_ptr<T>
Make_unique(Args&& ... args)
{
	return unique_ptr<T>(new T(forward<Args>(args)...));
}

//递归结束=======================================
template <typename T>
constexpr auto sum(T x)
{
	return x;
}

template <typename T1, typename T2, typename... Targ>
constexpr auto sum(T1 x, T2 y, Targ... args)
{
	return sum(x + y, args...);
}
//===============================================

//***********************************************
//map函数
template < template <typename, typename> class OutContainer = vector, typename F, class R>
auto fmap(F&& f, R&& inputs)
{
	typedef decay_t<decltype(f(*inputs.begin()))> result_type;
	OutContainer< result_type, allocator<result_type>> result;
	for (auto&& item : inputs) {
		result.push_back(f(item));
	}
	return result;
}
//返回的是lambda表达式
auto square_list =
[](auto&& container) {
	return fmap([](int x) { return x * x; }, container);
};

//返回的是lambda表达式
auto sum_list =
[](auto&& container) {
	return accumulate(
		container.begin(),
		container.end(), 0);
};

//组合模板compose的参数是函数
template <typename F, typename... Args>
auto compose(F f, Args... other)
{
	return[f, other...](auto&& ... x) {//x为调用时传入的参数；参数和调用时传入的参数不要搞混了；
		return f(compose(other...)(
			forward<decltype(x)>(x)...));
	};
}

//递归终止
template <typename F>
auto compose(F f)
{
	return [f](auto&& ... x) {
		return f(
			forward<decltype(x)>(x)...);
	};
}

auto squared_sum = compose(sum_list, square_list);

//***********************************************


int main() {
	auto ptr = Make_unique<vector<int>>(100, 1);
	auto result = sum(2, 3, 4, 5, 6);
	printf("%d \n", result);

	vector v{ 1, 2, 3, 4, 5 };
	cout << squared_sum(v) << endl;
}

