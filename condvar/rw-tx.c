#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "futex.h"
#include "condvar.h"
#include "buf-m-2.h"



cond_event_t done_cond;

#define NUM_READ 1
#define NUM_WRITE 1
int num_read = NUM_READ;
int num_write = NUM_WRITE;
int iter = 100;
int sleep_frac = 2;
int read_sleep = 1000;
int write_sleep = 1000;


void * reader(void * ev)
{
  int tid = (int) ev;
  __int64 tsc;
  while (1) { // !done) {
    __int64 value;
    value = GET();
    tsc = _rdtsc();
    printf("%d: Got delta %f, size=%d\n",tid, ((double) (tsc - value)) / (double) 3000000000.0,GETSIZE());
    usleep(read_sleep);
  }
  printf("Thread exiting\n");
  return(NULL);
}


void * writer(void * ev)
{
  int tid = (int) ev;
  int i,j;
  sleep (1);
  for (i = 0; i < iter; i++) {
    __int64 val;
    if ((rand() % sleep_frac) == 0) {
      usleep(write_sleep);
    }

    val = _rdtsc(); // rand();
    printf("%d:%d Put size=%d\n",tid, i, GETSIZE());

    PUT(val);
  }
  cond_begin;
  num_write --;
  if (num_write  == 0) {
    cond_event_tm_signal(&done_cond);
  }
  cond_end;
  return(NULL);
}


int main(int argc, char * argv[])
{
  pthread_t thd1 = 0;
  int tid1;
  pthread_t thd2 = 0;
  int tid2;
  int i;

  cond_event_init(&done_cond);
  if (argc >= 3) {
    num_read = strtol(argv[1], NULL, 10);
    num_write = strtol(argv[2], NULL, 10);

  }
  if (argc >= 4) {
    iter = strtol(argv[3], NULL, 10);
  }
  if (argc >= 5) {
    sleep_frac = strtol(argv[4], NULL, 10);
  }
  if (argc >= 6) {
    read_sleep = strtol(argv[5], NULL, 10);
  }
  if (argc >= 7) {
    write_sleep = strtol(argv[6], NULL, 10);
  }

  printf("Readers = %d, writers = %d, iterations = %d, sleep_frac = %d, read_sleep = %d, write_sleep = %d\n",
	 num_read,num_write, iter, sleep_frac, read_sleep, write_sleep);
  for (i = 0; i < num_read; i++) {
    tid1 = pthread_create(&thd1, NULL, reader, (void *) i);
  }
  for (i = 0; i < num_write; i++) {
    tid2 = pthread_create(&thd2, NULL, writer, (void *) i);
  }

  sleep(1);

  cond_begin;
  {
    while (num_write != 0) {
      cond_wait(&done_cond);
    }
  } 
  cond_end;
  sleep(7);
  return(0);
}
