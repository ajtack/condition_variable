#include <iostream>

using std::cout;
using std::endl;

void f();
void g_prime();
void g();
void h_prime();
void h();

int main()	{
	f();
}

void f()
{
	cout << "f() {" << endl;
	cout << "\t..." << endl;
	g();
	cout << "\t..." << endl;
	cout << "\t__tm_atomic" << endl;
	cout << "\t{" << endl;
	__tm_atomic
	{
		cout << "\t\t..." << endl;
		g_prime();
		cout << "\t\t..." << endl;
	}
	cout << "\t}" << endl;
	cout << "\t..." << endl;
	cout << "}" << endl;
}

__attribute__((tm_callable)) void g_prime()
{
	cout << "\t\tg'() {" << endl;
	cout << "\t\t\t..." << endl;
	h_prime();
	cout << "\t\t\t..." << endl;
	cout << "\t\t}" << endl;
}

void g()
{
	cout << "\t\tg() {" << endl;
	cout << "\t\t\t..." << endl;
	h();
	cout << "\t\t\t..." << endl;
	cout << "\t\t}" << endl;
}

__attribute__((tm_callable)) void h_prime()
{
	cout << "\t\t\th'() {" << endl;
	cout << "\t\t\t\t..." << endl;
	cout << "\t\t\t\t// do some work." << endl;
	cout << "\t\t\t\t..." << endl;
	cout << "\t\t\t}" << endl;
}

void h()
{
	cout << "\t\t\th() {" << endl;
	cout << "\t\t\t\t..." << endl;
	cout << "\t\t\t\t// do some work." << endl;
	cout << "\t\t\t\t..." << endl;
	cout << "\t\t\t}" << endl;
}
