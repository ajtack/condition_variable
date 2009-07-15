#ifndef ENVIRONMENT_H_WYJOI89K
#define ENVIRONMENT_H_WYJOI89K

#include "label.h"
#include <stdbool.h>
#include "condvar/condvar.h"

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
	label current_downcall;
	cond_event_t* current_condition;
} condition_variable_environment_t;


#ifndef TM_ATOMIC
#define TM_ATOMIC(tmid, code)                                                 \
	{                                                                         \
		condition_variable_environment_t env_##tmid =                         \
			{ .active = false, .current_downcall = NULL };                    \
		                                                                      \
		STORE_STACK_BASE_AND_PIC(env_##tmid.outer_frame.stack_base,           \
		                         env_##tmid.outer_frame.pic);                 \
		                                                                      \
		do {                                                                  \
			__tm_atomic {                                                     \
				if (env_##tmid.active)                                        \
					GOTO(env_##tmid.current_downcall);                        \
					                                                          \
				code                                                          \
				                                                              \
			tmid##_end:                                                       \
			}                                                                 \
			                                                                  \
			if (env_##tmid.active) {                                          \
				cond_begin;                                                   \
				cond_wait(env_##tmid.current_condition);                      \
				cond_end;                                                     \
			}                                                                 \
		} while(env_##tmid.active);                                           \
	}                                                                         \

#endif

#ifndef condition_variable_environment_call
#define condition_variable_environment_call(tmid, function, arguments...)                  \
        condition_variable_environment_call_unique(tmid, __COUNTER__, function, arguments) \

#endif

#ifndef condition_variable_environment_call_unique
#define condition_variable_environment_call_unique(tmid, counter, function, arguments...)  \
{                                                                                          \
MAKE_LABEL(before_, function, counter):                                                    \
                                                                                           \
    if (env_##tmid.active) {                                                               \
        RESTORE_STACK_AND_PIC_AND_GOTO(env_##tmid.inner_frame.stack_base,                  \
        env_##tmid.inner_frame.pic, env_##tmid.inner_frame.jump_point);                    \
    } else {                                                                               \
        env_##tmid.outer_frame.jump_point = &&MAKE_LABEL(after_, function, counter);       \
        function##_prime(&env_##tmid, ##arguments);                                        \
    }                                                                                      \
                                                                                           \
MAKE_LABEL(after_, function, counter):                                                     \
    if (env_##tmid.active) {                                                               \
        env_##tmid.current_downcall = &&MAKE_LABEL(before_, function, counter);            \
        goto tmid##_end;                                                                   \
    } else {                                                                               \
        env_##tmid.current_downcall = NULL;                                                \
    }                                                                                      \
}                                                                                          \

#endif

#ifndef condition_variable_environment_local_wait
#define condition_variable_environment_local_wait(tmid, condition)                     \
        condition_variable_environment_local_wait_unique(tmid, __COUNTER__, condition) \

#endif


#ifndef condition_variable_environment_local_wait_unique
#define condition_variable_environment_local_wait_unique(tmid, counter, condition)                \
{                                                                                                 \
    env_##tmid.current_downcall = ADDRESS_OF_LABEL(MAKE_LABEL(after_wait_, __LINE__, counter));   \
    env_##tmid.current_condition = condition;                                                     \
    env_##tmid.active = true;                                                                     \
    goto tmid##_end;                                                                              \
MAKE_LABEL(after_wait_, __LINE__, counter):                                                       \
    env_##tmid.current_condition = NULL;                                                          \
    env_##tmid.active = false;                                                                    \
}                                                                                                 \
	
#endif

#ifndef condition_variable_environment_wait
#define condition_variable_environment_wait(environment, condition)                           \
{                                                                                             \
    STORE_STACK_BASE_AND_PIC(environment->inner_frame.stack_base,                             \
                             environment->inner_frame.pic);                                   \
    environment->inner_frame.jump_point =                                                     \
                             ADDRESS_OF_LABEL(JOIN(_wait_line_, __LINE__));                   \
    environment->current_condition = condition;                                               \
    environment->active = true;                                                               \
    SET_STACK_AND_PIC_AND_GOTO(environment->outer_frame.stack_base,                           \
                               environment->outer_frame.pic,                                  \
                               environment->outer_frame.jump_point);                          \
                                                                                              \
JOIN(_wait_line_, __LINE__):                                                                  \
    environment->active = false;                                                              \
}                                                                                             \
	
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

#endif /* end of include guard: ENVIRONMENT_H_WYJOI89K */
