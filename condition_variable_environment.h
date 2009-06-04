#include "label.h"
#include <stdbool.h>


typedef struct frame
{
	label stack_base;
	label jump_point;
	label pic;
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
		STORE_STACK_BASE_AND_PIC(env_##tmid.outer_frame.stack_base,        \
		                         env_##tmid.outer_frame.pic);              \
		                                                                   \
		do {                                                               \
			fprintf(stderr, "\t__tm_atomic	{\n");                         \
				code                                                       \
				                                                           \
			tmid##_end:                                                    \
				first_time = false;                                        \
				                                                           \
				fprintf(stderr, "\t}\n");                                  \
				if (env_##tmid.active) {                                   \
					fprintf(stderr, "\t\t// WAIT!\n");                     \
				}                                                          \
		} while(env_##tmid.active);                                        \
	}                                                                      \

#endif


#ifndef condition_variable_environment_call
#define condition_variable_environment_call(tmid, function)                           \
	if (first_time)	{                                                                 \
		fprintf(stderr, "\t\t// before function ...\n");                              \
		env_##tmid.outer_frame.jump_point = &&MAKE_LABEL(after_, function, __LINE__); \
		function##_prime(&env_##tmid);                                                \
	}                                                                                 \
	else	{                                                                         \
		RESTORE_STACK_AND_PIC_AND_GOTO(env_##tmid.inner_frame.stack_base,             \
		    env_##tmid.inner_frame.pic, env_##tmid.inner_frame.jump_point);           \
	}                                                                                 \
	                                                                                  \
	MAKE_LABEL(after_, function, __LINE__):                                           \
		if (!env_##tmid.active) {                                                     \
			fprintf(stderr, "\t\t// after function ...\n");                           \
		}                                                                             \
		else {                                                                        \
			goto tmid##_end;                                                          \
		}                                                                             \

#endif

#ifndef JOIN
#define JOIN(x, y) JOIN_AGAIN(x, y)
#endif

#ifndef JOIN_AGAIN
#define JOIN_AGAIN(x, y) x ## y
#endif

#ifndef MAKE_LABEL
#define MAKE_LABEL(name, x, y) JOIN(name, JOIN(x, y))
#endif
