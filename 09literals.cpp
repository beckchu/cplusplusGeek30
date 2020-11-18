#include<iostream>
#include<bitset>
//自定义字面常量
using namespace std;

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

struct length {
	double value;

	static constexpr double factors[] =
	{ 1.0,    1000.0,  1e-3,
	 1e-2,   0.0254,  0.3048,
	 0.9144, 1609.344 };

	explicit length(double v, unit u = metre)
	{
		value = v * factors[u];
	}

	double get_v() {
		return value;
	}
};

length operator+(length lhs, length rhs)
{
	return length(lhs.value +
		rhs.value);
}


length operator"" _m(long double v)
{
	return length(v, unit::metre);
}

length operator"" _cm(long double v)
{
	return length(v, unit::centimetre);
}

int main() {
	cout << length(1.0, unit::kilometre).get_v() << endl;
	cout << (1.0_m + 10.0_cm).get_v() << endl;

	unsigned mask = 0b111'000'000;//c++14 对应文件权限
	long r_earth_equatorial = 6'378'137;
	double pi = 3.14159'26535'89793;
	const unsigned magic = 0x44'42'47'4E;//对应字节双字节
	cout << bitset<9>(mask) << endl;
}
