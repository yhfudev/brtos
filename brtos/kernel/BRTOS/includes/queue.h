#ifndef OS_QUEUE_H
#define OS_QUEUE_H

#include "OS_types.h"


// Habilita estruturas de fila
#define OS_Q_EN              1
#define OS_MAX_Q             2

#define READ_BUFFER_OK       0
#define BUFFER_UNDERRUN      1
#define CLEAN_BUFFER_OK      2


/*
*********************************************************************************************************
*                                          MESSAGE QUEUE DATA
*********************************************************************************************************
*/

typedef struct os_queue
{
  INT8U   *OSQPtr;
  INT8U   *OSQStart;
  INT8U   *OSQEnd;
  INT8U   *OSQIn;
  INT8U   *OSQOut;
  INT16U  OSQSize;
  INT16U  OSQEntries;
} OS_QUEUE;


void OSQueue(OS_QUEUE *cqueue,INT16U size);
INT8U OSWQueue(OS_QUEUE *cqueue,INT8U data);
INT8U OSRQueue(OS_QUEUE *cqueue);

#endif