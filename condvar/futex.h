#ifndef __FUTEX_H__
#define __FUTEX_H__

typedef struct cond_event {
  int event;
  int loops;
} cond_event_t;

#define ESCAPE __attribute__((tm_pure)) 
#define TM  __attribute__((tm_callable))
#define TM_CALLABLE __attribute__((tm_callable))
#define TM_SAFE __attribute__((tm_safe))
#define TM_PURE __attribute__((tm_pure))

ESCAPE int futex_wait(void * futex, int val);
ESCAPE int futex_wake(void * futex, int nwake);


ESCAPE int cond_event_init(cond_event_t * ev);
ESCAPE int cond_event_signal(cond_event_t * ev);
ESCAPE int cond_event_wait(cond_event_t * ev);
ESCAPE int cond_event_broadcast(cond_event_t * ev);
TM int cond_event_get(cond_event_t * ev);
ESCAPE int cond_event_deferred_wait(cond_event_t * ev, int val);


ESCAPE int cond_event_tm_signal_body(cond_event_t * ev) ;
TM int cond_event_tm_signal(cond_event_t * ev) ;
ESCAPE void cond_event_inc(cond_event_t * ev) ;

#endif // __FUTEX_H__
