#include "label.h"
#include <stdbool.h>


typedef struct frame
{
	label stack_base;
	label jump_point;
} frame_t;


typedef struct condition_variable_environment
{	
	frame_t inner_frame;
	frame_t outer_frame;
	bool active;
} condition_variable_environment_t;


#ifndef TM_ATOMIC
#define TM_ATOMIC(tmid, code)                                              \
	{                                                                      \
		bool first_time = true;                                            \
		condition_variable_environment_t env_##tmid = { .active = false }; \
		                                                                   \
		do {                                                               \
			fprintf(stderr, "__tm_atomic	{\n");                         \
				code                                                       \
				                                                           \
			tmid##_end:                                                    \
				first_time = false;                                        \
			fprintf(stderr, "}\n");                                        \
		} while(env_##tmid.active);                                       \
	}                                                                      \

#endif



#ifndef condition_variable_environment_call
#define condition_variable_environment_call(tmid, function, arguments...) \
	    condition_variable_environment_call_(tmid, cve_tmid, __LINE__, function, arguments)
#endif


#ifndef condition_variable_environment_call_
#define condition_variable_environment_call_(tmid, cve, function, line, arguments...)  \
	if (first_time)	{                                                                  \
		fprintf(stderr, "\t\t// before function ...\n");                               \
		cve.outer_frame.jump_point = &&after_##function##_##line;                      \
		function##_prime(arguments);                                                           \
	}                                                                                  \
	else	{                                                                          \
		RESTORE_STACK_AND_GOTO(env.inner.stackBase, env.inner.jumpPoint);              \
	}                                                                                  \
	                                                                                   \
	&&after_##function##_##line:                                                       \
		if (!cve.active())                                                             \
			fprintf(stderr, "\t\t// after function ...\n");                            \
		else                                                                           \
			goto tmid##_end;                                                           \

#endif
