
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

// Habilita o serviço de semáforo do sistema
#define BRTOS_SEM_EN           1

// Habilita o serviço de mutex do sistema
#define BRTOS_MUTEX_EN         1

// Habilita o serviço de mailbox do sistema
#define BRTOS_MBOX_EN          1

// Habilita o serviço de filas do sistema
#define BRTOS_QUEUE_EN         1

// Define o número máximo de semáforos (limita a alocação de memória p/ semáforos)
#define BRTOS_MAX_SEM          4

// Define o número máximo de mutex (limita a alocação de memória p/ mutex)
#define BRTOS_MAX_MUTEX        2

// Define o número máximo de Mailbox (limita a alocação de memória p/ mailbox)
#define BRTOS_MAX_MBOX         1

// Define o número máximo de filas (limita a alocação de memória p/ filas)
#define BRTOS_MAX_QUEUE        1

// Define o número máximo de tarefas que podem ser acordadas pelo evento
#define WAIT_LIST_SIZE  4

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
/////    Semaphore Control Block Structure             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct {
  INT8U  OSEventAllocated;                    // Indicate allocated event
  INT8U  OSEventCount;                        // Semaphore Count
  INT8U  OSEventWait;                         // Counter of waiting Tasks
  INT8U  OSEventList[WAIT_LIST_SIZE];         // Task wait list for event to occur
} BRTOS_Sem;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Mutex Control Block Structure                 /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct {
  INT8U  OSEventAllocated;                    // Indicate allocated event
  INT8U  OSEventCount;                        // Semaphore Count
  INT8U  OSEventOwner;                        // Used to Mutex - Define mutex owner
  INT8U  OSMaxPriority;                       // Used to Mutex - Define max priority accessing resource
  INT8U  OSOriginalPriority;                  // Used to Mutex - Save original priority of Mutex owner task
  INT8U  OSEventWait;                         // Counter of waiting Tasks
  INT8U  OSEventList[WAIT_LIST_SIZE];         // Task wait list for event to occur  
} BRTOS_Mutex;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    MailBox Control Block Structure               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct {
  INT8U  OSEventAllocated;                    // Indicate allocated event
  void   *OSEventPointer;                     // Pointer to message structure
  INT8U  OSEventWait;                         // Counter of waiting Tasks
  INT8U  OSEventList[WAIT_LIST_SIZE];         // Task wait list for event to occur
} BRTOS_Mbox;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Queue Control Block Structure                 /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

typedef struct {
  INT8U  OSEventAllocated;                    // Indicate allocated event
  INT8U  OSEventCount;                        // Semaphore Count
  INT8U  OSEventWait;                         // Counter of waiting Tasks  
  void   *OSEventPointer;                     // Pointer to message structure
  INT8U  OSEventList[WAIT_LIST_SIZE];         // Task wait list for event to occur
} BRTOS_Queue;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Event Variables Extern Declarations         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_SEM_EN == 1)
  extern BRTOS_Sem BRTOS_Sem_Table[BRTOS_MAX_SEM];
#endif

#if (BRTOS_MUTEX_EN == 1)
  extern BRTOS_Mutex BRTOS_Mutex_Table[BRTOS_MAX_MUTEX];
#endif

#if (BRTOS_MBOX_EN == 1)
  extern BRTOS_Mbox BRTOS_Mbox_Table[BRTOS_MAX_MBOX];
#endif

#if (BRTOS_QUEUE_EN == 1)
  extern BRTOS_Queue BRTOS_Queue_Table[BRTOS_MAX_QUEUE];
#endif

void initEvents(void);

#if (BRTOS_SEM_EN == 1)
  INT8U OSSemCreate (INT8U cnt, BRTOS_Sem **event);
  INT8U OSSemPend (BRTOS_Sem *pont_event, INT16U timeout);
  INT8U OSSemPost(BRTOS_Sem *pont_event);
#endif

#if (BRTOS_MUTEX_EN == 1)
  INT8U OSMutexCreate (BRTOS_Mutex **event, INT8U HigherPriority);
  INT8U OSMutexPend (BRTOS_Mutex *pont_event, INT16U timeout);
  INT8U OSMutexPost(BRTOS_Mutex *pont_event);  
#endif

#if (BRTOS_MBOX_EN == 1)
  INT8U OSMboxCreate (BRTOS_Mbox **event, void *message);
  void *OSMboxPend (BRTOS_Mbox *pont_event, INT16U timeout);
  INT8U OSMboxPost(BRTOS_Mbox *pont_event, void *message);
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif
