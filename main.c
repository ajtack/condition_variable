#include "condition_variable_environment.h"
#include "condvar/condvar.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>


// Some global condition which allows us to check actual waiting.
static cond_event_t the_global_condition;

void* run_f(void*);
void* signal_condition(void*);
void f();
void g_prime(condition_variable_environment_t* const env);
void g();
void h_prime(condition_variable_environment_t* const env);
void h();
void wait(int, int);


int main()	{
	pthread_t signaler;
	pthread_t waiter;
	
	pthread_create(&waiter, NULL, run_f, NULL);
	pthread_create(&signaler, NULL, signal_condition, &the_global_condition);
	
	pthread_join(waiter, NULL);
	pthread_kill(signaler, SIGQUIT);
	pthread_join(signaler, NULL);
	
	return 0;
}


void* run_f(void* ignored)
{
	f();
	return NULL;
}


void* signal_condition(void* condition)
{
	while(true)	{
		sleep(1);
		cond_event_tm_signal(condition);
	}
	
	return NULL;
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
	fprintf(stderr, "\t__tm_atomic {\n");
	TM_ATOMIC(A,
		fprintf(stdout, "\t\tMidagi siia läheb...\n");
		condition_variable_environment_call(A, g)
		fprintf(stdout, "\t\tKa siia läheb midagi...\n");
	)
	fprintf(stderr, "\t}\n");

	fprintf(stderr, "\t// ...\n");
	fprintf(stderr, "\tFunktsioon f on valmis!\n");
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
	fprintf(stderr, "\t\t...\n");
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
	condition_variable_environment_wait(env, &the_global_condition);
	fprintf(stderr, "\t\t\t\tAfter first wait() call... \n");
	condition_variable_environment_wait(env, &the_global_condition);
	fprintf(stderr, "\t\t\t\tAfter second wait() call... \n");
	fprintf(stderr, "\t\t\t\t...\n");
	fprintf(stderr, "\t\t\t}\n");
	return;
}
