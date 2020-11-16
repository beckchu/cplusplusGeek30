#include<utility>
#include<cstdio>

//计数类
class shared_count {
public:
	shared_count() noexcept :count_(1) {}
	void add_count() noexcept {
		++count_;
	}
	long reduce_count() noexcept {
		return --count_;
	}
	long get_count() const noexcept {
		return count_;
	}
private:
	long count_;
};

template <typename T>
class smart_ptr {
public:
	template<typename U> friend class smart_ptr;//拷贝构造函数要用到，friend可以访问private成员变量；

	explicit smart_ptr(T* ptr = nullptr) :ptr_(ptr) {
		puts("actor...");
		if (ptr) {
			shared_count_ = new shared_count();//堆上的数据
		}
	}

	~smart_ptr() {
		if (ptr_ && !shared_count_->reduce_count()) {
			delete ptr_;
			delete shared_count_;
		}
	}
	//拷贝构造函数，类型相同；
	smart_ptr(const smart_ptr& other) {//并不是noexcept的，可能出现异常；
		puts("copy actor...");
		ptr_ = other.ptr_;
		if (ptr_) {
            other.shared_count_->add_count();
            shared_count_=other.shared_count_;
		}
	}
	//拷贝构造函数，类型推断；
	template<typename U> smart_ptr(const smart_ptr<U>& other) noexcept {//noexcept
		ptr_ = other.ptr_;
		if (ptr_) {
			other.shared_count_->add_count();
			shared_count_ = other.shared_count_;
		}
	}
	//移动构造函数,类型推断；
	template<typename U> smart_ptr(smart_ptr<U>&& other) noexcept {
		puts("move actor...");
		ptr_ = other.ptr_;
		if (ptr_) {
		shared_count_=other.shared_count_;
			other.ptr_ = nullptr;
		}
	}
	//构造函数；强制类型转换中使用，比较特殊；
	template<typename U> smart_ptr(const smart_ptr<U>& other, T* ptr) noexcept {
		ptr_ = ptr;
		if (ptr_) {
			other.shared_count_->add_count();
			shared_count_ = other.shared_count_;
		}
	}
	//拷贝赋值函数；这样的写法保证了强异常安全性；
	//强异常安全性：在临时变量上做所有的操作，然后swap，
	//异常出现在（1），
	smart_ptr& operator=(smart_ptr rhs) noexcept {//by-value，拷贝构造函数failed可以保证*this的state， （1）
		puts("operator= actor...");
		rhs.swap(*this);//swap是noexcept的（2）
		return *this;
	}

	T* get() const noexcept {
		return ptr_;
	}

	long use_count() const noexcept {
        if(ptr_){
            return shared_count_->get_count();
        }else{
            return 0;
        }
	}

	void swap(smart_ptr& rhs) noexcept {//不抛异常
		using std::swap;
		swap(ptr_, rhs.ptr_);
        swap(shared_count_,rhs.shared_count_);
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

private:
    T* ptr_;
    shared_count* shared_count_;
};

template<typename T> void swap(smart_ptr<T>& lhs, smart_ptr<T>& rhs) noexcept {
	lhs.swap(rhs);
}

//cast	函数；
template<typename T, typename U>
smart_ptr<T> static_pointer_cast(const smart_ptr<U>& other) noexcept {
	T* ptr = static_cast<T*>(other.get());
	return smart_ptr<T>(other, ptr);
}

template<typename T, typename U>
smart_ptr<T> reinterpret_pointer_cast(const smart_ptr<U>& other) noexcept {
	T* ptr = reinterpret_cast<T*>(other.get());
	return smart_ptr<T>(other, ptr);
}

template<typename T, typename U>
smart_ptr<T> const_pointer_cast(const smart_ptr<U>& other) noexcept {
	T* ptr = const_cast<T*>(other.get());
	return smart_ptr<T>(other, ptr);
}

template<typename T, typename U>
smart_ptr<T> dynamic_pointer_cast(const smart_ptr<U>& other) noexcept {
	T* ptr = dynamic_cast<T*>(other.get());
	return smart_ptr<T>(other, ptr);
}

class shape{
public:
    virtual ~shape(){}
};

class circle:public shape{
public:
    ~circle(){puts("~circle");}
};

int main(){
    smart_ptr<circle> ptr1(new circle());
    printf("use count of ptr1 is %ld\n",ptr1.use_count());
    smart_ptr<shape> ptr2;
    ptr2=ptr1;   
    printf("use count of ptr2 is %ld\n",ptr2.use_count());

    if(ptr1){
        puts("ptr1 is not empty");
    }
    smart_ptr<circle> ptr3=dynamic_pointer_cast<circle>(ptr2);
    printf("use count of ptr3 is %ld\n",ptr3.use_count());
}
