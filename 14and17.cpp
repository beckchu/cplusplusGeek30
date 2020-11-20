#include <stdio.h>
#include<vector>
#include <iostream>
#include <numeric>

using namespace std;

//这里使用的是重载
template <typename T>
struct has_reserve {
	//good 和bad只要不一样就可以
	struct good { char dummy; };
	struct bad { char dummy[2]; };
	//类的函数指针，写法有点怪；
	template <class U, void (U::*)(size_t)> struct SFINAE {};
	//SFINAE*这个类型无所谓的；下面是函数重载，不关心返回值类型；
	template <class U> static good reserve(SFINAE<U, &U::reserve>*);
	//任意参数
	template <class U> static bad reserve(...);
	//函数返回值的大小，第一次见这种写法；
	static const bool value = sizeof(reserve<T>(nullptr)) == sizeof(good);
};

//用这个方法来避免模板函数特化的坑；这里的函数应该也包含成员函数；
template<class T> struct FImpl;

template<class T>
void f(T t) { FImpl<T>::f(t); } // users, don't touch this!

template<class T>
struct FImpl
{
	static void f(T t); // users, go ahead and specialize this 
};


// filter_view 的定义
template<typename _InIt, typename _Fun>
class filter_view {
public:
	class iterator { // 实现 InputIterator 
	public:

		using iterator_category = input_iterator_tag;
		using value_type = typename _InIt::value_type;
		using difference_type = typename _InIt::difference_type;
		using pointer = typename _InIt::pointer;
		using reference = value_type &;

		iterator(_InIt _First, _InIt _Last, _Fun f)
			:_First(_First), _Last(_Last), _fun(f) {
			++(*this);
		}

		reference operator*() const noexcept { return *_Cur; }

		pointer operator->() const noexcept { return &(*_Cur); }

		iterator& operator++() {
			while (_First != _Last && !_fun(*_First)) {
				_First++;
			}
			_Cur = _First;
			if (_First != _Last) {
				_First++;
			}
			return *this;
		}

		iterator operator++(int) {
			iterator temp(*this);
			++(*this);
			return temp;
		}

		bool operator==(const iterator& rhs)
			const noexcept
		{
			return _Cur == rhs._Cur;
		}
		bool operator!=(const iterator& rhs)
			const noexcept
		{
			return !operator==(rhs);
		}
	private:
		_InIt _First;
		_InIt _Last;
		_InIt _Cur;
		_Fun _fun;

	};

	filter_view(_InIt _First, _InIt _Last, _Fun f)
		:_First(_First), _Last(_Last), _fun(f) {

	}

	iterator begin() const noexcept {
		return iterator(_First, _Last, _fun);
	}

	iterator end() const noexcept {
		return iterator(_Last, _Last, _fun);
	}
private:
	_InIt _First;
	_InIt _Last;
	_Fun _fun;
};


