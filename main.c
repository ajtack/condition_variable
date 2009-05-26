#include <stdio.h>
#include "condition_variable_environment.h"

void f();
void g_prime(condition_variable_environment_t* const env);
void g();
void h_prime(condition_variable_environment_t* const env);
void h();
void wait(int, int);

int main()	{
	f();
	return 0;
}


void f()
{
	fprintf(stderr, "f() {\n");
	fprintf(stderr, "\t// before a non-transactional call to g ...\n");
	g();
	fprintf(stderr, "\t// after a non-transactional call to g ...\n");
	fprintf(stderr, "\n");
	
	// This block...
	//
	// 	fprintf(stderr, "\t__tm_atomic {\n")
	// 	__tm_atomic {
	//		fprintf(stderr, "\t\t// before g() ...\n");
	// 		g();
	//		fprintf(stderr, "\t\t// after g() ...\n");
	// 	}
	//	fprintf(stderr, "\t}\n");
	//
	// Becomes:
	
	TM_ATOMIC(A,
		fprintf(stderr, "HI.\n");
	);

	fprintf(stderr, "\t// ...\n");
	fprintf(stderr, "}\n");
}


/*
 * g() -- Non-Transactional version.
 *
 * This is what the programmer writes. Ideally, in an annotated STM, the tm_callable
 * attribute will suffice.
 */
void g()
{
	fprintf(stderr, "\tg() {\n");
	fprintf(stderr, "\t\t...\n");
	h();
	h();
	fprintf(stderr, "\t\t...\n");
	fprintf(stderr, "\t}\n");
}


/*
 * g() -- Transactional Version
 *
 * This is generated from g() by the compiler.
 */
__attribute__((tm_callable)) void g_prime(condition_variable_environment_t* const env)
{
	fprintf(stderr, "\t\tg'() {\n");
	fprintf(stderr, "\t\t\t...\n");
	
	// Call to h is replaced with...
	h_prime(env);
	
	fprintf(stderr, "\t\t\t...\n");
	
	// Call to h is replaced with...
	h_prime(env);

	fprintf(stderr, "\t\t\t...\n");
	fprintf(stderr, "\t\t}\n");
}


/*
 * h() -- Non-Transactional version.
 *
 * This is what the programmer writes.
 */
void h()
{
	fprintf(stderr, "\t\th() {\n");
	fprintf(stderr, "\t\t\t...\n");
	fprintf(stderr, "\t\t\twait();\n");
	fprintf(stderr, "\t\t\t...\n");
	fprintf(stderr, "\t\t}\n");
}


/*
 * h() -- Transactional version.
 *
 * This is generated from h() by the compiler.
 */
__attribute__((tm_callable)) void h_prime(condition_variable_environment_t* const env)
{		
	fprintf(stderr, "\t\t\th'() {\n");
	fprintf(stderr, "\t\t\t\t...\n");

	// In place of the wait() ...
	{
		env->inner_frame.jump_point = ADDRESS_OF_LABEL(after_wait_1);
		env->active = true;
		SET_STACK_AND_GOTO(env->inner_frame.stack_base, env->outer_frame.stack_base, env->outer_frame.jump_point);
		
	after_wait_1:
		env->active = false;
	}
	
	fprintf(stderr, "\t\t\t\tAfter first wait() call... \n");
	
	// In place of the wait() ...
	{
		env->inner_frame.jump_point = ADDRESS_OF_LABEL(after_wait_2);
		env->active = true;
		SET_STACK_AND_GOTO(env->inner_frame.stack_base, env->outer_frame.stack_base, env->outer_frame.jump_point);
		
	after_wait_2:
		env->active = false;
	}
	
	fprintf(stderr, "\t\t\t\tAfter second wait() call... \n");
	fprintf(stderr, "\t\t\t}\n");
	return;
}
