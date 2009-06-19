/* ---------------------------------------------------------------- */
/* MONITOR  Buffer:                                                 */
/*    This file simulates a bounded buffer.  It consists of the     */
/* following procedures:                                            */
/*    (1)  BufferInit() : initialize the monitor                    */
/*    (2)  GET()        : retrieve the next available item          */
/*    (3)  PUT()        : insert an item into the buffer            */
/* ---------------------------------------------------------------- */

#include  "buf-m-2.h"
#include "condvar.h"

#define   BUF_SIZE   5                  /* buffer size              */

static __int64   Buf[BUF_SIZE];             /* the actual buffer        */
static int   in;                        /* next empty location      */
static int   out;                       /* next filled location     */
static int   count;                     /* no. of items in buffer   */


static cond_event_t   UntilNotFull;           /* wait until full cv       */
static cond_event_t   UntilNotEmpty;          /* wait until full cv       */

/* ---------------------------------------------------------------- */
/* FUNCTION  BufferInit():                                          */
/*   This function initializes the buffer                           */
/* ---------------------------------------------------------------- */

void  BufferInit(void)
{
     in = out = count = 0;

     cond_event_init(&UntilNotFull);
     cond_event_init(&UntilNotEmpty);
}

/* ---------------------------------------------------------------- */
/* FUNCTION  GET():                                                 */
/*   This function retrieves the next available item from the buffer*/
/* ---------------------------------------------------------------- */

__int64  GET(void)
{
  __int64  value;

  cond_begin;

    while (count == 0) {            /* while nothing in buffer  */
      //
      // Set up for the wait
      //
      
      cond_wait(&UntilNotEmpty);
    }
      
    value = Buf[out];             /* retrieve an item         */
    out = (out + 1) % BUF_SIZE;   /* advance out pointer      */
    count--;                      /* decrease counter         */
    cond_event_tm_signal(&UntilNotFull);   /* signal consumers         */
   cond_end;
  return  value;                     /* return retrieved vague   */
}

/* ---------------------------------------------------------------- */
/* FUNCTION  BufferInit():                                          */
/*   This function adds an item to the buffer                       */
/* ---------------------------------------------------------------- */

__int64  PUT(__int64  value)
{
  cond_begin;
  {
    while (count == BUF_SIZE) {     /* while buffer is full     */
      cond_wait(&UntilNotFull); /* then wait */
    }	
    Buf[in] = value;              /* add the value to buffer  */
    in = (in + 1) % BUF_SIZE;     /* advance in pointer       */
    count++;                      /* increase count           */
    cond_event_tm_signal(&UntilNotEmpty);  /* signal producers         */
  }
  cond_end;
  return  value;                     /* return the value         */
}

int GETSIZE(void)
{
  return(count);
}
