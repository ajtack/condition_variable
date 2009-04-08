#include <iostream>
#include <stack>
#include <setjmp.h>

using std::cout;
using std::endl;
using std::stack;

#define tm_begin cout << "\t__tm_atomic" << endl; \
                 cout << "\t{" << endl;

#define tm_end   cout << "\t}" << endl;

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
	cout << "\t// before g ..." << endl;
	g();
	cout << "\t// after g ..." << endl;
	
	// In place of a regular call to g' (which would be as above)
	{
		bool waiting = false;
		bool first_time = true;
		stack<label> jump_points;
		
		do {
			tm_begin	{
				if (first_time)
					cout << "\t\t// before g' ..." << endl;
				
				g_prime(waiting, jump_points);
				
				if (!waiting)
					cout << "\t\t// after g' ..." << endl;
			}
			tm_end
			
			if (waiting)
				cout << "\twait(...)" << endl;
			
			first_time = false;
		} while(waiting);
	}

	cout << "\t..." << endl;
	cout << "}" << endl;
}

__attribute__((tm_callable)) void g_prime(bool& am_waiting, stack<label>& jump_points)
{
	if (am_waiting)	{
		label continuation = jump_points.top();
		jump_points.pop();
		goto *continuation;
	}
	else	{
		cout << "\t\tg'() {" << endl;
		cout << "\t\t\t..." << endl;
	}
	
	// Call to h' is replaced with...
	waitable_function_call_1:
	{
		h_prime(am_waiting, jump_points);

		if (am_waiting)	{
			jump_points.push(&&waitable_function_call_1);
			return;
		}
	}
	
	cout << "\t\t\t..." << endl;
	
	// Call to h' is replaced with...
	waitable_function_call_2:
	{
		h_prime(am_waiting, jump_points);

		if (am_waiting)	{
			jump_points.push(&&waitable_function_call_2);
			return;
		}
	}

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
	if (am_waiting)	{
		label continuation = jump_points.top();
		jump_points.pop();
		goto *continuation;
	}
	else	{
		cout << "\t\t\th'() {" << endl;
		cout << "\t\t\t\t..." << endl;
	}

	// In place of the wait() ...
	{
		jump_points.push(&&after_wait_1);
		am_waiting = true;
		return;
		
	after_wait_1:
		am_waiting = false;
	}
	
	cout << "\t\t\t\t..." << endl;
	
	// In place of the wait() ...
	{
		jump_points.push(&&after_wait_2);
		am_waiting = true;
		return;
		
	after_wait_2:
		am_waiting = false;
	}
	
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
