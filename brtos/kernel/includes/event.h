
/*********************************************************************************************************
*                                               BRTOS
*                                Brazilian Real-Time Operating System
*                            Acronymous of Basic Real-Time Operating System
*
*                              
*                                  Open Source RTOS under MIT License
*
*
*
*                                    OS Event Handle Structs Header
*
*
*   Author: Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#ifndef BRTOSEvent_H
#define BRTOSEvent_H


#include "OS_types.h"





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Event Defines                            /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Habilita o gerenciamento de eventos do sistema
#define BRTOSEvent_EN          1

// Define o número máximo de eventos (limita a alocação de memória p/ eventos)
#define BRTOS_MAX_EVENTS       8

// Define o número máximo de tarefas que podem ser acordadas pelo evento
#define WAIT_LIST_SIZE  8


// Define semaphore types
#define BRTOSEvent_UNASSIGNED  0
#define BRTOSEvent_TYPE_SEM    1
#define BRTOSEvent_TYPE_MUTEX  2
#define BRTOSEvent_TYPE_MBOX   3
#define BRTOSEvent_TYPE_QUEUE  4


// Condições da alocação de blocos de controle de eventos
#define ALLOC_EVENT_OK       (INT8U)0
#define NO_AVAILABLE_EVENT   (INT8U)1
#define IRQ_PEND_ERR         (INT8U)2
#define ERR_EVENT_TYPE       (INT8U)3
#define ERR_SEM_OVF          (INT8U)4
#define ERR_MUTEX_OVF        (INT8U)4
#define ERR_EVENT_NO_CREATED (INT8U)5
#define ERR_EVENT_OWNER      (INT8U)6

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Event Control Block Structure                 /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct {
  void   *OSEventPointer;                     // Pointer to message structure
  INT8U  OSEventCount;                        // Semaphore Count
  INT8U  OSEventType;                         // BRTOS Event type (Semaphore, Mutex, Mailbox and Queue
  INT8U  OSEventWait;                         // Counter of waiting Tasks
  INT8U  OSEventList[WAIT_LIST_SIZE];         // Task wait list for event to occur
  INT8U  OSEventOwner;                        // Used to Mutex - Define mutex owner
  INT8U  OSMaxPriority;                       // Used to Mutex - Define max priority accessing resource
  INT8U  OSOriginalPriority;                  // Used to Mutex - Save original priority of Mutex owner task
} BRTOSEvent;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Event Variables Extern Declarations         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

extern BRTOSEvent BRTOSEventTable[BRTOS_MAX_EVENTS];
extern INT16U iBRTOSEvent;
extern void *OSEventFreeList;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif
