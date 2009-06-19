//#include <stdio.h>
//#include <unistd.h>
//#include <stdlib.h>
#include <limits.h>
//#include <pthread.h>
//#include <linux/futex.h>
#include "lowlevellock.h"
#include "futex.h"

ESCAPE int futex_wait(void *futex, int val)
{
  int result;

  result = lll_futex_wait(futex, val);

  return(result);
}

ESCAPE int futex_wake(void * futex, int nwake)
{
  int result;
  
  //  result = 
    lll_futex_wake(futex, nwake);

  return(0);
}


ESCAPE int cond_event_init(cond_event_t * ev)
{
  ev->event = 0;
  ev->loops = 0;
  return(0);
}

ESCAPE int cond_event_signal(cond_event_t * ev) 
{
  ev->event++;
  futex_wake(&ev->event, 1);
  return(0);
}

ESCAPE int cond_event_tm_signal_body(cond_event_t * ev) 
{
  futex_wake(&ev->event, 1);
  return(0);
}

TM int cond_event_tm_signal(cond_event_t * ev) 
{
  ev->event++;
  cond_event_tm_signal_body(ev);
  return(0);
}

ESCAPE int cond_event_broadcast(cond_event_t * ev)
{
  ev->event++;
  futex_wake(&ev->event, INT_MAX);
  return(0);
}

ESCAPE int cond_event_wait(cond_event_t * ev)
{
  futex_wait(&ev->event, ev->event);
  return(0);
}

TM int cond_event_get(cond_event_t * ev)
{
  return(ev->event);
}

ESCAPE int cond_event_deferred_wait(cond_event_t * ev, int val)
{
  if (val == ev->event) {
    futex_wait(&ev->event, val);
  }
  return(0);
}

ESCAPE void cond_event_inc(cond_event_t * ev) 
{
  ev->loops++;
}
