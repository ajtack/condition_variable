#include <iostream>
#include <stack>
#include <setjmp.h>
#include "condition_variable_environment.hxx"
#include "tm_emulate.hxx"

void f();
void g_prime(condition_variable_environment& env);
void g();
void h_prime(condition_variable_environment& env);
void h();
void wait(int, int);

int main()	{
	f();
	return 0;
}

using std::cout;
using std::endl;
using std::stack;

void f()
{
	
	cout << "f() {" << endl;
	cout << "\t// before g ..." << endl;
	g();
	cout << "\t// after g ..." << endl;
	
	// In place of a regular call to g' (which would be as above)
	{
		bool first_time = true;
		condition_variable_environment env;
		SET_STACK_BASE(env.outer.stackBase);
		
		do {
			tm_begin
			__tm_atomic	{
				if (first_time)	{
					cout << "\t\t// before g' ..." << endl;
					env.outer.jumpPoint = &&after_g_prime;
					g_prime(env);
				}
				else	{
					RESTORE_STACK_AND_GOTO(env.inner.stackBase, env.inner.jumpPoint);
				}
					
			after_g_prime:
				if (!env.active())
					cout << "\t\t// after g' ..." << endl;
			}
			tm_end
			
			if (env.active())
				wait(7, 3);
			
			first_time = false;
		} while(env.active());
	}

	cout << "\t..." << endl;
	cout << "}" << endl;
}

__attribute__((tm_callable)) void g_prime(condition_variable_environment& env)
{
	cout << "\t\tg'() {" << endl;
	cout << "\t\t\t..." << endl;
	
	// Call to h' is replaced with...
	h_prime(env);
	
	cout << "\t\t\t..." << endl;
	
	// Call to h' is replaced with...
	h_prime(env);

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

__attribute__((tm_callable)) void h_prime(condition_variable_environment& env)
{
	cout << "\t\t\th'() {" << endl;
	cout << "\t\t\t\t..." << endl;

	// In place of the wait() ...
	{
		env.activate(&&after_wait_1);
		SET_STACK_AND_GOTO(env.inner.stackBase, env.outer.stackBase, env.outer.jumpPoint);
		
	after_wait_1:
		env.deactivate();
	}
	
	cout << "\t\t\t\t..." << endl;
	
	// In place of the wait() ...
	{
		env.activate(&&after_wait_2);
		SET_STACK_AND_GOTO(env.inner.stackBase, env.outer.stackBase, env.outer.jumpPoint);
		
	after_wait_2:
		env.deactivate();
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

void wait(int x, int y)
{
	cout << "\twait(...)" << endl;
}
