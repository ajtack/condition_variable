#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "futex.h"


cond_event_t event = {0};

void * thread_func(void * ev)
{
  cond_event_t * event = (cond_event_t *)ev;

  printf("About to wait!\n");
  cond_event_wait(ev);
  printf("Done waiting\n");
}

int main(int argc, char * argv[])
{
  pthread_t thd = 0;
  int tid;
  cond_event_init(&event);
  tid = pthread_create(&thd, NULL, thread_func, &event);
  sleep(2);
  printf("About to signal\n");
  cond_event_signal(&event);
  printf("Did signal\n");
  sleep(2);
  return(0);
}
