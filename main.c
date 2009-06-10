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
	fprintf(stderr, "f()\n{\n");
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
	{
		_Bool first_time = 1;
		condition_variable_environment_t env_A = { .active = 0 };
		asm("movl %%ebp, %0;" "movl %%ebx, %1;"
		    : "=m" (env_A.outer_frame.stack_base), "=m" (env_A.outer_frame.pic)
		    :
		    : "memory");
		;
		
		do {
			fprintf(__stderrp, "\t__tm_atomic\n\t{\n");
			
			if (first_time) {
				fprintf(__stderrp, "\t\t// before function ...\n");
				env_A.outer_frame.jump_point = &&after_g38;
				g_prime(&env_A);
			} else {
				asm volatile ("movl %0, %%eax;" "movl %1, %%ebp;" "movl %2, %%ebx;" "jmp *%%eax;" "popa;" : : "r" (env_A.inner_frame.jump_point), "r" (env_A.inner_frame.stack_base), "r" (env_A.inner_frame.pic) : "memory", "eax");
				;
			}
			
		after_g38:
			if (!env_A.active) {
				fprintf(__stderrp, "\t\t// after function ...\n");
			} else {
				goto A_end;
			};
			
		A_end:
			first_time = 0;
			fprintf(__stderrp, "\t}\n");
			if (env_A.active) {
				fprintf(__stderrp, "\t\t// WAIT!\n");
			}
		} while(env_A.active);
	}
 

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
	fprintf(stderr, "\tg()\n\t{\n");
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
	fprintf(stderr, "\t\tg'()\n\t\t{\n");
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
	fprintf(stderr, "\t\th()\n\t\t{\n");
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
	fprintf(stderr, "\t\t\th'()\n\t\t\t{\n");
	fprintf(stderr, "\t\t\t\t...\n");

	// In place of the wait() ...
	{
		STORE_STACK_BASE_AND_PIC(env->inner_frame.stack_base, env->inner_frame.pic);
		env->inner_frame.jump_point = ADDRESS_OF_LABEL(after_wait_1);
		env->active = true;
		SET_STACK_AND_PIC_AND_GOTO(env->outer_frame.stack_base, env->outer_frame.pic, env->outer_frame.jump_point);
		
	after_wait_1:
		env->active = false;
	}
	
	fprintf(stderr, "\t\t\t\tAfter first wait() call... \n");
	
	// In place of the wait() ...
	{
		STORE_STACK_BASE_AND_PIC(env->inner_frame.stack_base, env->inner_frame.pic);
		env->inner_frame.jump_point = ADDRESS_OF_LABEL(after_wait_2);
		env->active = true;
		SET_STACK_AND_PIC_AND_GOTO(env->outer_frame.stack_base, env->outer_frame.pic, env->outer_frame.jump_point);
		
	after_wait_2:
		env->active = false;
	}
	
	fprintf(stderr, "\t\t\t\tAfter second wait() call... \n");
	fprintf(stderr, "\t\t\t\t...\n");
	fprintf(stderr, "\t\t\t}\n");
	return;
}
