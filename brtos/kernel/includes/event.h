/**
* \file event.h
* \brief BRTOS Event Handler defines, structs declarations and functions prototypes.
*
*
**/

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
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.2
*   Date:     01/10/2010
*
*********************************************************************************************************/

#ifndef BRTOSEvent_H
#define BRTOSEvent_H


#include "OS_types.h"
#include "BRTOS.h"





///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////                 OS User Event Defines               /////
/////                                                     /////
/////             !User configuration defines!            /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/// Enable or disable semaphore controls
#define BRTOS_SEM_EN           1

/// Enable or disable mutex controls
#define BRTOS_MUTEX_EN         1

/// Enable or disable mailbox controls
#define BRTOS_MBOX_EN          1

/// Enable or disable queue controls
#define BRTOS_QUEUE_EN         1

/// Enable or disable queue 16 bits controls
#define BRTOS_QUEUE_16_EN      1

/// Enable or disable queue 32 bits controls
#define BRTOS_QUEUE_32_EN      1

/// Defines the maximum number of semaphores\n
/// Limits the memory allocation for semaphores
#define BRTOS_MAX_SEM          4

/// Defines the maximum number of mutexes\n
/// Limits the memory allocation for mutex
#define BRTOS_MAX_MUTEX        4

/// Defines the maximum number of mailboxes\n
/// Limits the memory allocation mailboxes
#define BRTOS_MAX_MBOX         1

/// Defines the maximum number of queues\n
/// Limits the memory allocation for queues
#define BRTOS_MAX_QUEUE        3





///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////               OS BRTOS Event Defines                /////
/////                                                     /////
/////  !Do not change unless you know what you're doing!  /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

// Return defines to events control blocks
#define ALLOC_EVENT_OK          (INT8U)0      ///< Event allocated with success
#define NO_AVAILABLE_EVENT      (INT8U)1      ///< No event control blocks available
#define IRQ_PEND_ERR            (INT8U)2      ///< Function can not be called inside an interrupt
#define ERR_SEM_OVF             (INT8U)3      ///< Semaphore counter overflow
#define ERR_MUTEX_OVF           (INT8U)4      ///< Mutex counter overflow
#define ERR_EVENT_NO_CREATED    (INT8U)5      ///< There are no task waiting for the event
#define ERR_EVENT_OWNER         (INT8U)6      ///< Function caller is not the owner of the event control block. Used to mutex implementation
#define DELETE_EVENT_OK         (INT8U)7      ///< Event deleted with success
#define AVAILABLE_RESOURCE      (INT8U)8      ///< The resource is available
#define BUSY_RESOURCE           (INT8U)9      ///< The resource is busy
#define AVAILABLE_MESSAGE       (INT8U)10     ///< There is a message
#define NO_MESSAGE              (INT8U)11     ///< There is no message


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Semaphore Control Block Structure             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/**
* \struct BRTOS_Sem
* Semaphore Control Block Structure
*/
typedef struct {
  INT8U  OSEventAllocated;                    ///< Indicate if the event is allocated or not
  INT8U  OSEventCount;                        ///< Semaphore Count - This value is increased with a post and decremented with a pend
  INT8U  OSEventWait;                         ///< Counter of waiting Tasks
  #if NUMBER_OF_PRIORITIES == 32
  INT32U OSEventWaitList;                     ///< Task wait list for event to occur
  #else
  INT16U OSEventWaitList;                     ///< Task wait list for event to occur
  #endif
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

/**
* \struct BRTOS_Mutex
* Mutex Control Block Structure
*/
typedef struct {
  INT8U  OSEventAllocated;                    ///< Indicate if the event is allocated or not
  INT8U  OSEventState;                        ///< Mutex state - Defines if the resource is available or not
  INT8U  OSEventOwner;                        ///< Defines mutex owner
  INT8U  OSMaxPriority;                       ///< Defines max priority accessing resource
  INT8U  OSOriginalPriority;                  ///< Save original priority of Mutex owner task - used to the priority ceiling implementation
  INT8U  OSEventWait;                         ///< Counter of waiting Tasks
  #if NUMBER_OF_PRIORITIES == 32
  INT32U OSEventWaitList;                     ///< Task wait list for event to occur
  #else
  INT16U OSEventWaitList;                     ///< Task wait list for event to occur
  #endif
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

/**
* \struct BRTOS_Mbox
* MailBox Control Block Structure
*/
typedef struct {
  INT8U  OSEventAllocated;                    ///< Indicate if the event is allocated or not
  INT8U  OSEventWait;                         ///< Counter of waiting Tasks
  INT8U  OSEventState;                        ///< Mailbox state - Defines if the message is available or not
  #if NUMBER_OF_PRIORITIES == 32
  INT32U OSEventWaitList;                     ///< Task wait list for event to occur
  #else
  INT16U OSEventWaitList;                     ///< Task wait list for event to occur
  #endif
  void   *OSEventPointer;                     ///< Pointer to the message structure / type
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

/**
* \struct BRTOS_Queue
* Queue Control Block Structure
*/
typedef struct {
  INT8U  OSEventAllocated;                    ///< Indicate if the event is allocated or not
  INT8U  OSEventCount;                        ///< Queue Event Count - This value is increased with a post and decremented with a pend
  INT8U  OSEventWait;                         ///< Counter of waiting Tasks
  void   *OSEventPointer;                     ///< Pointer to queue structure
  #if NUMBER_OF_PRIORITIES == 32
  INT32U OSEventWaitList;                     ///< Task wait list for event to occur
  #else
  INT16U OSEventWaitList;                     ///< Task wait list for event to occur
  #endif
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
  /// Semahore Control Block
  extern BRTOS_Sem BRTOS_Sem_Table[BRTOS_MAX_SEM];
#endif

#if (BRTOS_MUTEX_EN == 1)
  /// Mutex Control Block
  extern BRTOS_Mutex BRTOS_Mutex_Table[BRTOS_MAX_MUTEX];
#endif

#if (BRTOS_MBOX_EN == 1)
  /// MailBox Control Block
  extern BRTOS_Mbox BRTOS_Mbox_Table[BRTOS_MAX_MBOX];
#endif

#if (BRTOS_QUEUE_EN == 1)
  /// Queue Control Block
  extern BRTOS_Queue BRTOS_Queue_Table[BRTOS_MAX_QUEUE];
#endif


/*****************************************************************************************//**
* \fn void initEvents(void)
* \brief Initialize event control blocks
* \return NONE
*********************************************************************************************/  
void initEvents(void);


#if (BRTOS_SEM_EN == 1)

  /*****************************************************************************************//**
  * \fn INT8U OSSemCreate (INT8U cnt, BRTOS_Sem **event)
  * \brief Allocates a semaphore control block
  * \param cnt Initial Semaphore counter - default = 0
  * \param **event Address of the semaphore control block pointer
  * \return IRQ_PEND_ERR Can not use semaphore create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No semaphore control blocks available
  * \return ALLOC_EVENT_OK Semaphore control block successfully allocated
  *********************************************************************************************/
  INT8U OSSemCreate (INT8U cnt, BRTOS_Sem **event);
  
  /*****************************************************************************************//**
  * \fn INT8U OSSemDelete (BRTOS_Sem **event)
  * \brief Releases a semaphore control block
  * \param **event Address of the semaphore control block pointer
  * \return IRQ_PEND_ERR Can not use semaphore delete function from interrupt handler code
  * \return DELETE_EVENT_OK Semaphore control block released with success
  *********************************************************************************************/
  INT8U OSSemDelete (BRTOS_Sem **event);

  /*****************************************************************************************//**
  * \fn INT8U OSSemPend (BRTOS_Sem *pont_event, INT16U timeout)
  * \brief Wait for a semaphore post
  *  Semaphore pend may be used to syncronize tasks or wait for an event occurs.
  *  A task exits a pending state with a semaphore post or by timeout.
  * \param *pont_event Semaphore pointer
  * \param timeout Timeout to the semaphore pend exits
  * \return OK Success
  * \return TIMEOUT There was no post for this semaphore in the specified time
  * \return IRQ_PEND_ERR Can not use semaphore pend function from interrupt handler code
  * \return NO_EVENT_SLOT_AVAILABLE Full Event list
  *********************************************************************************************/
  INT8U OSSemPend (BRTOS_Sem *pont_event, INT16U timeout);
  
  /*****************************************************************************************//**
  * \fn INT8U OSSemPost(BRTOS_Sem *pont_event)
  * \brief Semaphore post
  *  Semaphore Post may be used to syncronize tasks or to inform that an event occurs.
  * \param *pont_event Semaphore pointer
  * \return OK Success
  * \return ERR_SEM_OVF Semaphore counter overflow
  *********************************************************************************************/  
  INT8U OSSemPost(BRTOS_Sem *pont_event);
#endif

#if (BRTOS_MUTEX_EN == 1)
  /*****************************************************************************************//**
  * \fn INT8U OSMutexCreate (BRTOS_Mutex **event, INT8U HigherPriority)
  * \brief Allocates a mutex control block
  * \param **event Address of the mutex control block pointer
  * \param HigherPriority Higher priority of the tasks that will share a resource
  * \return IRQ_PEND_ERR Can not use mutex create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No mutex control blocks available
  * \return ALLOC_EVENT_OK Mutex control block successfully allocated
  *********************************************************************************************/
  INT8U OSMutexCreate (BRTOS_Mutex **event, INT8U HigherPriority);
  
  /*****************************************************************************************//**
  * \fn INT8U OSMutexDelete (BRTOS_Mutex **event)
  * \brief Releases a mutex control block
  * \param **event Address of the mutex control block pointer
  * \return IRQ_PEND_ERR Can not use mutex delete function from interrupt handler code
  * \return DELETE_EVENT_OK Mutex control block released with success
  *********************************************************************************************/  
  INT8U OSMutexDelete (BRTOS_Mutex **event);

  /*****************************************************************************************//**
  * \fn INT8U OSMutexAcquire(BRTOS_Mutex *pont_event)
  * \brief Wait for a mutex release
  *  Mutex release may be used to manage shared resources, for exemple, a LCD.
  *  A acquired state exits with a mutex owner release
  * \param *pont_event Mutex pointer
  * \return OK Success
  * \return IRQ_PEND_ERR Can not use mutex pend function from interrupt handler code
  * \return NO_EVENT_SLOT_AVAILABLE Full Event list
  *********************************************************************************************/
  INT8U OSMutexAcquire(BRTOS_Mutex *pont_event);

  /*****************************************************************************************//**
  * \fn INT8U OSMutexRelease(BRTOS_Mutex *pont_event)
  * \brief Release Mutex
  *  Mutex release must be used to release a shared resource.
  *  Only the mutex owner can executed the mutex post function with success.
  * \param *pont_event Mutex pointer
  * \return OK Success
  * \return ERR_EVENT_OWNER The function caller is not the mutex owner
  * \return ERR_MUTEX_OVF Mutex counter overflow
  *********************************************************************************************/  
  INT8U OSMutexRelease(BRTOS_Mutex *pont_event);
#endif

#if (BRTOS_MBOX_EN == 1)

  /*****************************************************************************************//**
  * \fn INT8U OSMboxCreate (BRTOS_Mbox **event, void *message)
  * \brief Allocates a mailbox control block
  * \param **event Address of the mailbox control block pointer
  * \param *message Specifies an initial message for the allocated mailbox control block
  * \return IRQ_PEND_ERR Can not use mailbox create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No mailbox control blocks available
  * \return ALLOC_EVENT_OK Mailbox control block successfully allocated
  *********************************************************************************************/
  INT8U OSMboxCreate (BRTOS_Mbox **event, void *message);
  
  /*****************************************************************************************//**
  * \fn INT8U OSMboxDelete (BRTOS_Mbox **event)
  * \brief Releases a mailbox control block
  * \param **event Address of the mailbox control block pointer
  * \return IRQ_PEND_ERR Can not use mailbox delete function from interrupt handler code
  * \return DELETE_EVENT_OK Mailbox control block released with success
  *********************************************************************************************/  
  INT8U OSMboxDelete (BRTOS_Mbox **event);
  
  /*****************************************************************************************//**
  * \fn void *OSMboxPend (BRTOS_Mbox *pont_event, INT16U timeout)
  * \brief Wait for a message post
  *  Mailbox pend may be used to receive messages from tasks and interrupts.
  *  A task exits a pending state with a mailbox post or by timeout.
  *  A message could be of any kind of data type.
  * \param *pont_event Mailbox pointer
  * \param *Mail Mail content pointer
  * \param timeout Timeout to the mailbox pend exits
  * \return OK Success
  * \return TIMEOUT There was no post for this semaphore in the specified time
  * \return IRQ_PEND_ERR Can not use semaphore pend function from interrupt handler code
  *********************************************************************************************/  
  INT8U OSMboxPend (BRTOS_Mbox *pont_event, void *Mail, INT16U timeout);
  
  /*****************************************************************************************//**
  * \fn INT8U OSMboxPost(BRTOS_Mbox *pont_event, void *message)
  * \brief Mailbox post
  *  Mailbox post may be used to send messages to tasks.
  *  A message could be of any kind of data type.
  * \param *pont_event Semaphore pointer
  * \param *message Pointer to the message to be sent
  * \return OK Success
  * \return ERR_EVENT_NO_CREATED No tasks waiting for the message
  *********************************************************************************************/  
  INT8U OSMboxPost(BRTOS_Mbox *pont_event, void *message);
#endif


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif
