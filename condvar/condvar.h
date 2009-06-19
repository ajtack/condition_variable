#ifndef __CONDVAR_H__
#define __CONDVAR_H__

#include "futex.h"
#pragma warning(disable:177) 

#define BEGIN_TX __tm_atomic
#define END_TX

//#define BEGIN_TX

//#define cond_event_inc_loops(_ev_)
#define cond_event_inc_loops(_ev_) cond_event_inc(_ev_)
#define LABEL_CAT(a, b) LABEL_CAT_I(a, b)
#define LABEL_CAT_I(a, b) a ## b
#define NEXT_LABEL(T) LABEL_HELPER(T,LABEL_CAT(label,__LINE__))
#define LABEL_HELPER(T,L)  do { T = &&L; L: __dummy = 0;} while(0)

#define cond_begin \
 { \
   int __do_wait = 0, __cond_val, __do_exit = 0; \
   int __dummy = 0;\
   cond_event_t * __cond_var; \
   void * __wait_label; \
   void * __begin_label; \
   do { \
   if (__do_wait) { \
    cond_event_deferred_wait(__cond_var,__cond_val); \
  } \
   BEGIN_TX { \
    NEXT_LABEL(__begin_label); \    
    if (!__do_exit) { \      
    if (__do_wait) { \
      __do_wait = 0; \
      goto *__wait_label; \
    } 

#define cond_end \
    } else {__do_exit = 0;} } \
END_TX; \
 } while (__do_wait); }


#define cond_wait_label( _cvar_, _label_) \
	{ \
          __cond_var = (_cvar_);		\
  	  __cond_val = cond_event_get(_cvar_);	\
	  __do_wait = 1; \
           __do_exit = 1; \
           __wait_label = && _label_; \
	  goto *__begin_label; \
	_label_:  \
	  __dummy = 0;	   \
	}

#define cond_wait( _cvar_) \
      do { \
       cond_event_inc_loops( _cvar_ ); \
       cond_wait_label( _cvar_, LABEL_CAT(__cond_wait_, __LINE__)); \
      } while (0)
  
#endif // __CONDVAR_H__


/* 
 
 Notes on condition variables:

- currently requires a system call on all signals. 
- Problem: how do you atomically test if anyone is waiting? Just read # of waiters, if zero can ignore?
*/ 
