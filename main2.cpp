#include "my_function.h"
#include <bits/stdc++.h>

using namespace std;

tuple<int, int, int> g(int a, int& b, const int& c) {
	tuple<int, int, int> t{a, b, c};
	a = 10, b = 10;
	cout << "gggggggggggggggg\n";
	return t;
}

void h(int&& a) {
	cout << a << '\n';
	a = 11;
}

struct op {
	int a, b, c, d, e, f;
	void operator()() {
		cout << "hi\n";
	}
};

struct op2 {
	int a, b;
	void operator()() {
		cout << "hi_2\n";
	}
};

int main(){
	int a, b, c;
	my_function<tuple<int, int, int>(int, int&, const int&)> f1 = g;

	a = b = c = 5;
	auto t = f1(a, b, c);
	cout << get<0>(t) << ' ' << get<1>(t) << ' ' << get<2>(t) << ' ' << b << '\n';

	a = b = c = 5;
	t = f1(5, b, 5);
	cout << get<0>(t) << ' ' << get<1>(t) << ' ' << get<2>(t) << ' ' << b << '\n';

	my_function<void(int&&)> m = h;
	m(move(a));
	cout << a;

	my_function<void()> f2 = op(), f3 = op2();
	f2();
	f2 = std::move(f3);
	f2();
	f3();

	my_function<void()> f4 = op2(), f5 = op();
	f4();
	f4 = std::move(f5);
	f4();

	my_function<void()> f6 = op2(), f7 = op();
	f6();
	f7();
	f6 = f7;
	f6();
	f7();
}
