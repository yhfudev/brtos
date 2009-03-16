#ifndef OS_EVENT_H
#define OS_EVENT_H


#include "types.h"



// Habilita o gerenciamento de eventos do sistema
#define OS_EVENT_EN          1

// Define o número máximo de eventos (limita a alocação de memória p/ eventos)
#define OS_MAX_EVENTS        8

// Define o número máxima de tarefas que podem ser acordadas pelo evento
#define OS_EVENT_TABLE_SIZE  8


// Define semaphore types
#define OS_EVENT_TYPE_SEM    0
#define OS_EVENT_TYPE_MBOX   1
#define OS_EVENT_TYPE_Q      2
#define OS_EVENT_TYPE_MUTEX  3


// Condições da alocação de blocos de controle de eventos
#define ALLOC_EVENT_OK       (INT8U)0
#define NO_AVAILABLE_EVENT   (INT8U)1
#define IRQ_PEND_ERR         (INT8U)2
#define ERR_EVENT_TYPE       (INT8U)3
#define ERR_SEM_OVF          (INT8U)4
#define ERR_MUTEX_OVF        (INT8U)4
#define ERR_EVENT_NO_CREATED (INT8U)5
#define ERR_EVENT_OWNER      (INT8U)6


// Estrutura do bloco de controle de evento
typedef struct {
  void   *OSEventPtr;                       /* Ptr to message or queue structure */
  INT8U  OSEventTbl[OS_EVENT_TABLE_SIZE];   /* Wait list for event to occur */
  INT16U OSEventCnt;                        /* Count (when event is a semaphore) */
  INT8U  OSEventType;                       /* Event type */
  INT8U  OSEventGrp;                        /* Group count of waiting Tasks */
  INT8U  OSEventOwner;                      /* Used to Mutex - Define mutex owner */
  INT8U  OSMaxPriority;                     /* Used to Mutex - Define max priority accessing resource */
  INT8U  OSOriginalPriority;                /* Used to Mutex - Save original priority of Mutex owner task */
} OS_EVENT;

// Variáveis externas p/ controle de evento
extern OS_EVENT OSEventTbl[OS_MAX_EVENTS];
extern INT16U iOS_Event;
extern void *OSEventFreeList;

#endif