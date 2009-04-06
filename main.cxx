#include <iostream>
#include <stack>

using std::cout;
using std::endl;
using std::stack;

typedef void* label;

void f();
void g_prime(bool&, stack<label>&);
void g();
void h_prime(bool&, stack<label>&);
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
//	__tm_atomic
//	{
		bool waiting = false;
		stack<label> jump_points;
		cout << "\t\t..." << endl;
		g_prime(waiting, jump_points);
		
		while (waiting)	{
			cout << "-------------------------------------------" << endl;
			cout << "\t}" << endl;
			cout << "\twait(...)" << endl;
			cout << "\t__tm_atomic {" << endl;
			cout << "-------------------------------------------" << endl;
			g_prime(waiting, jump_points);
		}
		cout << "\t\t..." << endl;
//	}
	cout << "\t}" << endl;
	cout << "\t..." << endl;
	cout << "}" << endl;
}

__attribute__((tm_callable)) void g_prime(bool& am_waiting, stack<label>& jump_points)
{
	if (am_waiting)
		goto *jump_points.top();
	else	{
		cout << "\t\tg'() {" << endl;
		cout << "\t\t\t..." << endl;
	}
	
	h_prime(am_waiting, jump_points);
	
	if (am_waiting)	{
		jump_points.push(&&after_wait);
		return;
	}
	else	{
		goto after_h_prime;
	}
after_wait:
	jump_points.pop();
	h_prime(am_waiting, jump_points);

after_h_prime:
	cout << "\t\t\t..." << endl;
	cout << "\t\t}" << endl;
}

void g()
{
	cout << "\tg() {" << endl;
	cout << "\t\t..." << endl;
	h();
	cout << "\t\t..." << endl;
	cout << "\t}" << endl;
}

__attribute__((tm_callable)) void h_prime(bool& am_waiting, stack<label>& jump_points)
{
	if (am_waiting)
		goto *jump_points.top();
	else	{
		cout << "\t\t\th'() {" << endl;
		cout << "\t\t\t\t..." << endl;
	}

	// In place of the wait().
	jump_points.push(&&finish);
	am_waiting = true;
	return;
	
finish:
	jump_points.pop();
	am_waiting = false;
	cout << "\t\t\t\t..." << endl;
	cout << "\t\t\t}" << endl;
	return;
}

void h()
{
	cout << "\t\th() {" << endl;
	cout << "\t\t\t..." << endl;
	cout << "\t\t\twait();" << endl;
	cout << "\t\t\t..." << endl;
	cout << "\t\t}" << endl;
}
