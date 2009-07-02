/*!
 * \file
 * \brief Implements a demonstration of lockless conditional wait in transactions.
 *
 * The goals of this demonstration are:
 *  1. To demonstrate lockless conditional waits which escape the containing transaction
 *     (in place of releasing a lock)
 *  2. To demonstrate lockless waits to occurring deeper in the stack than the function
 *     which began the containing transaction
 *  3. To demonstrate how macros are applied to a semi-realistic code base to meet the
 *     previous two goals without adding much custom, brittle code to the program.
 *
 * \author Andres Jaan Tack (tack@cs.wisc.edu)
 */
#include "environment.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>


/*!
 * This variable is meant to indicate any condition which is accessible both to the
 * signaler and to the waiter. It has no particular meaning.
 */
static cond_event_t the_global_condition;


/*
 * We define here several functions which execute lockless waits in different scenarios,
 * both within and without a transaction.
 *
 * The reader will note that there are two versions of each function. The only difference:
 * one of them uses a standalone lockless wait, while the other (marked "_prime") uses a
 * lockless wait given an environment. This environment allows the wait to escape the
 * appropriate containing transaction.
 */

// This function drives the experiment, calling the below in
// various combinations with transactions.
void f();

// Functions taking no arguments:
//   g() calls h(),
void g();
void g_prime(condition_variable_environment_t* const env);

//   h() waits twice, escaping the transaction in f() to do so.
void h();
void h_prime(condition_variable_environment_t* const env);

// Functions taking some arguments
//   i() behaves as h(), but demonstrates how arguments are to
//       be shifted over to add the environment.
void i(int something, char something_else);
void i_prime(condition_variable_environment_t* const env, int something, char something_else);


/*!
 * Calls f(), runnable as a thread.
 *
 * \return NULL
 */
void* run_f(void*);


/*!
 * With a period of one second, signals the_global_condition.
 *
 * \return Never; expects to be killed by the spawning thread.
 */
void* signal_condition(void*);


/*!
 * \brief Runs the demonstration which this file describes.
 *
 * The demonstration is self-contained. No additional parameters are required.
 */
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


void f()
{
	fprintf(stderr, "f()\n{\n");
	fprintf(stderr, "\t// This section demonstrates correct output without transactions ...\n");
	g();
	fprintf(stderr, "\n");
	
	// This block...
	//
	// 	fprintf(stderr, "\t__tm_atomic {\n")
	// 	__tm_atomic {
	//		fprintf(stderr, "\t\t// ...\n");
	// 		g();
	//		fprintf(stderr, "\t\t// ...\n");
	// 	}
	//	fprintf(stderr, "\t}\n");
	//
	// Becomes:
	fprintf(stderr, "\t// This section demonstrates the same nested functions, but this time in a transaction.\n");
	fprintf(stderr, "\t__tm_atomic {\n");
	TM_ATOMIC(SOMETHING_UNIQUE,
		fprintf(stdout, "\t\t// Preamble to downcall to g() ...\n\n");
		condition_variable_environment_call(SOMETHING_UNIQUE, g);
		fprintf(stdout, "\n\t\t// Postamble to downcall to g() ...\n");
	)
	fprintf(stderr, "\t}\n");
	
	fprintf(stderr, "\n\t// This section demonstrates passing arguments to a similar function.\n");
	fprintf(stderr, "\t__tm_atomic {\n");
	TM_ATOMIC(SOMETHING_ELSE_UNIQUE,
		fprintf(stdout, "\t\t// Preamble to downcall to i() ...\n\n");
		condition_variable_environment_call(SOMETHING_ELSE_UNIQUE, i, 15, 'x');
		fprintf(stdout, "\n\t\t// Postamble to downcall to i() ...\n");
	)
	fprintf(stderr, "\t}\n");

	fprintf(stderr, "\n\tIf you see this, the demonstration succeeded!\n");
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
	cond_begin;
	cond_wait(&the_global_condition);
	cond_end;
	
	fprintf(stderr, "\t\t\t...\n");
	
	fprintf(stderr, "\t\t\twait();\n");
	cond_begin;
	cond_wait(&the_global_condition);
	cond_end;
	
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
	
	fprintf(stderr, "\t\t\t\twait();\n");
	condition_variable_environment_wait(env, &the_global_condition);
	
	fprintf(stderr, "\t\t\t\t...\n");
	
	fprintf(stderr, "\t\t\t\twait();\n");
	condition_variable_environment_wait(env, &the_global_condition);
	
	fprintf(stderr, "\t\t\t\t...\n");
	fprintf(stderr, "\t\t\t}\n");
	
	return;
}


/*!
 * i() -- Non-Transactional Version.
 */
void i(int something, char something_else)
{
	fprintf(stderr, "\th()\n\t\t{\n");
	fprintf(stderr, "\t\t...\n");
	
	fprintf(stderr, "\t\t\twait();\n");
	cond_begin;
	cond_wait(&the_global_condition);
	cond_end;
	
	fprintf(stderr, "\t\t...\n");
	
	fprintf(stderr, "\t\t\twait();\n");
	cond_begin;
	cond_wait(&the_global_condition);
	cond_end;
	
	fprintf(stderr, "\t\t...\n");
	fprintf(stderr, "\t}\n");
}


/*
 * i() -- Transactional version.
 */
void i_prime(condition_variable_environment_t* const env, int something, char something_else)
{		
	fprintf(stderr, "\t\ti'()\n\t\t{\n");
	fprintf(stderr, "\t\t\t...\n");
	
	fprintf(stderr, "\t\t\twait();\n");
	condition_variable_environment_wait(env, &the_global_condition);

	fprintf(stderr, "\t\t\t...\n");
	
	fprintf(stderr, "\t\t\twait();\n");
	condition_variable_environment_wait(env, &the_global_condition);
	
	fprintf(stderr, "\t\t\t...\n");
	fprintf(stderr, "\t\t}\n");
	
	return;
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