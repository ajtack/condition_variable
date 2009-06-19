/* ---------------------------------------------------------------- */
/* MONITOR  Buffer:                                                 */
/*    This file simulates a bounded buffer.  It consists of the     */
/* following procedures:                                            */
/*    (1)  BufferInit() : initialize the monitor                    */
/*    (2)  GET()        : retrieve the next available item          */
/*    (3)  PUT()        : insert an item into the buffer            */
/* ---------------------------------------------------------------- */

#ifndef   _CK_buffer_m_h
#define   _CK_buffer_m_h

void  BufferInit(void);                 /* initializes the monitor  */
__int64   GET(void);                        /* get an item from buffer  */
__int64   PUT(__int64  value);                  /* add an item to buffer    */
int GETSIZE(void);

#endif
