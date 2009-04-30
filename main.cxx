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

using std::cerr;
using std::cout;
using std::endl;
using std::stack;

void f()
{
	
	cerr << "f() {" << endl;
	cerr << "\t// before a non-transactional call to g ..." << endl;
	g();
	cerr << "\t// after a non-transactional call to g ..." << endl;
	cerr << endl;
	
	// In place of a regular call to g' (which would be as above)
	{
		bool first_time = true;
		condition_variable_environment env;
		SET_STACK_BASE(env.outer.stackBase);
		
		do {
			tm_begin
			__tm_atomic	{
				if (first_time)	{
					cerr << "\t\t// before g' ..." << endl;
					env.outer.jumpPoint = &&after_g_prime;
					g_prime(env);
				}
				else	{
					RESTORE_STACK_AND_GOTO(env.inner.stackBase, env.inner.jumpPoint);
				}
					
			after_g_prime:
				if (!env.active())
					cerr << "\t\t// after g' ..." << endl;
			}
			tm_end
			
			if (env.active())
				wait(7, 3);
			
			first_time = false;
		} while(env.active());
	}

	cerr << "\t..." << endl;
	cerr << "}" << endl;
}

__attribute__((tm_callable)) void g_prime(condition_variable_environment& env)
{
	cerr << "\t\tg'() {" << endl;
	cerr << "\t\t\t..." << endl;
	
	// Call to h' is replaced with...
	h_prime(env);
	
	cerr << "\t\t\t..." << endl;
	
	// Call to h' is replaced with...
	h_prime(env);

	cerr << "\t\t\t..." << endl;
	cerr << "\t\t}" << endl;
}

void g()
{
	cerr << "\tg() {" << endl;
	cerr << "\t\t..." << endl;
	h();
	cerr << "\t\t..." << endl;
	cerr << "\t}" << endl;
}

__attribute__((tm_callable)) void h_prime(condition_variable_environment& env)
{
	int array[1024];
	for (int i = 0; i < 1024; ++i)
		array[i] = 0xdeadbeef;
		
	cerr << "\t\t\th'() {" << endl;
	cerr << "\t\t\t\t..." << endl;

	// In place of the wait() ...
	{
		env.activate(&&after_wait_1);
		SET_STACK_AND_GOTO(env.inner.stackBase, env.outer.stackBase, env.outer.jumpPoint);
		
	after_wait_1:
		env.deactivate();
	}
	
	cerr << std::flush;
	cout << "\t\t\t\tAfter first wait() call: " << endl;
	for (int i = 0; i < 1024; ++i)
		if (array[i] != 0xdeadbeef) cout << "\t\t\t\tCorruption at index " << i << endl;
	cerr << "\t\t\t\t..." << endl;
	
	// In place of the wait() ...
	{
		env.activate(&&after_wait_2);
		SET_STACK_AND_GOTO(env.inner.stackBase, env.outer.stackBase, env.outer.jumpPoint);
		
	after_wait_2:
		env.deactivate();
	}
	
	cerr << std::flush;
	cout << "\t\t\t\tAfter second wait() call: " << endl;
	for (int i = 0; i < 1024; ++i)
		if (array[i] != 0xdeadbeef) cout << "\t\t\t\tCorruption at index " << i << endl;
	cerr << "\t\t\t\t..." << endl;
	cerr << "\t\t\t}" << endl;
	return;
}

void h()
{
	cerr << "\t\th() {" << endl;
	cerr << "\t\t\t..." << endl;
	cerr << "\t\t\twait();" << endl;
	cerr << "\t\t\t..." << endl;
	cerr << "\t\t}" << endl;
}

void wait(int x, int y)
{
	cerr << "\twait(...)" << endl;
}
