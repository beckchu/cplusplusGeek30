#include <iterator>   // std::back_inserter
#include <iostream>
#include <sstream>

using namespace std;

class istream_line_reader {
public:
	//内部类
	class iterator {
	public:
		typedef ptrdiff_t difference_type;
		typedef string value_type;
		typedef const value_type* pointer;
		typedef const value_type& reference;
		typedef input_iterator_tag iterator_category;

		iterator() noexcept : stream_(nullptr) {}

		explicit iterator(istream& is) : stream_(&is)
		{
			++* this;//保证ctor就会getline
		}
		//进行取值
		reference operator*() const noexcept
		{
			return line_;
		}

		pointer operator->() const noexcept
		{
			return &line_;
		}
		//进行读取，退出时stream会变为nullptr，和iterator::end()对应；
		iterator& operator++()
		{
			getline(*stream_, line_);
			if (!*stream_) {
				stream_ = nullptr;
			}
			return *this;
		}

		iterator operator++(int)
		{
			iterator temp(*this);
			++* this;
			return temp;
		}

		bool operator==(const iterator& rhs) const noexcept
		{
			return stream_ == rhs.stream_;
		}

		bool operator!=(const iterator& rhs) const noexcept
		{
			return !operator==(rhs);
		}

	private:
		istream* stream_;
		string line_;
	};

	istream_line_reader() noexcept : stream_(nullptr) {}
	explicit istream_line_reader(istream& is) noexcept : stream_(&is) {}
	iterator begin() { return iterator(*stream_); }
	iterator end() const noexcept { return iterator(); }

private:  istream* stream_;
};

int main() {
	istringstream s1{ "hello" };
	istream is(s1.rdbuf());
	for (const string& line : istream_line_reader(is)) {
		// 示例循环体中仅进行简单输出
		cout << line << endl;
	}
}
