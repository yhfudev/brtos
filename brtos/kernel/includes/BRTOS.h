
/**
* \file BRTOS.h
* \brief BRTOS kernel main defines, functions prototypes and structs declaration.
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
*                                          BRTOS Kernel Header
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#ifndef OS_BRTOS_H
#define OS_BRTOS_H

#include "OS_types.h"
#include "HAL.h"





///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////                   OS User Defines                   /////
/////                                                     /////
/////             !User configuration defines!            /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/// Define if simulation or DEBUG
#define DEBUG 0

/// Define if verbose info is available
#define VERBOSE 0

/// Define if whatchdog is active
#define WATCHDOG 1

/// Define if compute cpu load is active
#define COMPUTES_CPU_LOAD 1

// The Nesting define must be set in the file HAL.h
// Example:
/// Define if nesting interrupt is active
//#define NESTING_INT 0

/// Define Number of Priorities
#define NUMBER_OF_PRIORITIES 32

/// Define CPU Stack Pointer Size
#define SP_SIZE 32

/// Define the maximum number of Tasks to be Installed
/// must always be equal or higher to NumberOfInstalledTasks
#define NUMBER_OF_TASKS (INT8U)6

/// Define if OS Trace is active
#define OSTRACE 0

#if (OSTRACE == 1)  
  #include "debug_stack.h"
#endif

/// Define if TimerHook function is active
#define TIMER_HOOK_EN 1


/// TickTimer Defines
#define configCPU_CLOCK_HZ          (INT32U)25165824    ///< CPU clock in Hertz
#define configTICK_RATE_HZ          (INT32U)1000        ///< Tick timer rate in Hertz
#define configTIMER_PRE_SCALER      0                   ///< Informs if there is a timer prescaler
#define TickCountOverFlow           (INT16U)32000       ///< Determines the tick timer overflow
#define configRTC_CRISTAL_HZ        (INT32U)1000
#define configRTC_PRE_SCALER        10
#define OSRTCEN                     0
#define NO_TIMEOUT                  (INT16U)65000
#define EXIT_BY_TIMEOUT             (INT16U)65001



/// Stack Defines
/// Coldfire with 8KB of RAM: 40 * 128 bytes = 5KB of Virtual Stack
#define HEAP_SIZE 46*128

// Queue heap defines
// Configurado com 1KB p/ filas
#define QUEUE_HEAP_SIZE 8*128



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////                 OS BRTOS Defines                    /////
/////                                                     /////
/////  !Do not change unless you know what you're doing!  /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


/// False and True defines
#define FALSE (INT8U)0
#define TRUE  (INT8U)1

/// Task States
#define READY                        (INT8U)0     ///< Task is ready to be executed - waiting for the scheduler authorization
#define SUSPENDED                    (INT8U)1     ///< Task is suspended
#define BLOCKED                      (INT8U)2     ///< Task is blocked - Will not run until be released
#define MUTEX_PRIO                   (INT8U)0xFE
#define EMPTY_PRIO                   (INT8U)0xFF

/// Error codes
#define OK                           (INT8U)0     ///< OK define
#define NO_MEMORY                    (INT8U)1     ///< Error - Lack of memory to allocate a task
#define END_OF_AVAILABLE_PRIORITIES  (INT8U)2     ///< Error - There are no more priorities available
#define BUSY_PRIORITY                (INT8U)3     ///< Error - Priority is being used by another task
#define INVALID_TIME                 (INT8U)4     ///< Error - Informed time is out of the limits
#define TIMEOUT                      (INT8U)5     ///< Error - Timeout
#define PRIORITY_NOT_BLOCKED         (INT8U)6
#define CANNOT_ASSIGN_IDLE_TASK_PRIO (INT8U)7
#define NOT_VALID_TASK               (INT8U)8     ///< There current task number is not valid for this function


/// Suspended Types
#define DELAY     0                               ///< Task suspended by delay
#define SEMAPHORE 1                               ///< Task suspended by semaphore
#define MAILBOX   2                               ///< Task suspended by mailbox
#define QUEUE     3                               ///< Task suspended by queue
#define MUTEX     4                               ///< Task suspended by mutex



/// Task Defines
#define configMAX_TASK_NAME_LEN 16                ///< Defines the maximum number of characters in a task name

#if NUMBER_OF_PRIORITIES == 32
  #define configMAX_TASK_INSTALL  32                 ///< Defines the maximum number of tasks that can be installed
  #define configMAX_TASK_PRIORITY 31  
#else
  #define configMAX_TASK_INSTALL  16                 ///< Defines the maximum number of tasks that can be installed
  #define configMAX_TASK_PRIORITY 15
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Context Tasks Structure Prototypes          /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/**
* \struct ContextType
* Context Task Structure
* Used by the task control block
*/
struct Context
{
   const CHAR8 * TaskName;  ///< Task name
  #if SP_SIZE == 32
   INT32U StackPoint;       ///< Current position of virtual stack pointer
   INT32U StackInit;        ///< Virtual stack pointer init
  #else
   INT16U StackPoint;       ///< Current position of virtual stack pointer
   INT16U StackInit;        ///< Virtual stack pointer init  
  #endif
   INT16U TimeToWait;       ///< Time to wait - could be used by delay or timeout
  #if (VERBOSE == 1)
   INT8U  State;            ///< Task states
   INT8U  Blocked;          ///< Task blocked state
   INT8U  SuspendedType;    ///< Task suspended type
  #endif
   INT8U  Priority;         ///< Task priority
   struct Context *Next;
   struct Context *Previous;
};

typedef struct Context ContextType;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Functions Prototypes                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn void BRTOS_TimerHook(void)
* \brief Provide to the user a function sincronized with the timer tick
*  This function can be used to perform simple tests syncronized with the timer tick.
* \return NONE
*********************************************************************************************/  
void BRTOS_TimerHook(void);

/*****************************************************************************************//**
* \fn INT8U TaskStartScheduler(void)
* \brief Start the Operating System Scheduler
*  The user must call this function to start the tasks execution.
* \return OK Success
* \return NO_MEMORY There was not enough memory to start all tasks
*********************************************************************************************/
INT8U TaskStartScheduler(void);

/*****************************************************************************************//**
* \fn INT8U DelayTask(INT16U time)
* \brief Wait for a specified period.
*  A task that calling this function will be suspended for a certain time.
*  When this time is reached the task back to ready state.
* \param time Time in ticks to delay. System default = 1ms. The user can change the time value.
* \return OK Success
* \return IRQ_PEND_ERR - Can not use block priority function from interrupt handler code
*********************************************************************************************/
INT8U DelayTask(INT16U time);

/*****************************************************************************************//**
* \fn INT8U DelayTaskHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U miliseconds)
* \brief Wait for a specified period (in hours, minutes, seconds and miliseconds).
*  A task that calling this function will be suspended for a certain time.
*  When this time is reached the task back to ready state.
* \param hours Hours to delay
* \param minutes Minutes to delay
* \param seconds Seconds to delay
* \param miliseconds Miliseconds to delay
* \return OK Success
* \return INVALID_TIME The specified parameters are outside of the permitted range
*********************************************************************************************/  
INT8U DelayTaskHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U miliseconds);

/*****************************************************************************************//**
* \fn void PreInstallTasks(void)
* \brief Function that initialize the kernel main variables.
*  This function resets the kernel main variables, preparing the system to be started.
* \return NONE
*********************************************************************************************/  
void PreInstallTasks(void);

/*****************************************************************************************//**
* \fn INT8U BlockPriority(INT8U iPriority)
* \brief Blocks a specific priority
*  Blocks the task that is associated with the specified priority.
*  The user must be careful when using this function in together with mutexes.
*  This can lead to undesired results due the "cealing priority" property used in the mutex.
* \param iPriority Priority to be blocked
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use block priority function from interrupt handler code
*********************************************************************************************/  
INT8U BlockPriority(INT8U iPriority);

/*****************************************************************************************//**
* \fn INT8U UnBlockPriority(INT8U iPriority)
* \brief UnBlock a specific priority
*  UnBlocks the task that is associated with the specified priority.
*  The user must be careful when using this function in together with mutexes.
*  This can lead to undesired results due the "cealing priority" property used in the mutex.
* \param iPriority Priority to be unblocked
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use unblock priority function from interrupt handler code
*********************************************************************************************/
INT8U UnBlockPriority(INT8U iPriority);

/*****************************************************************************************//**
* \fn INT8U BlockTask(INT8U iTaskNumber)
* \brief Blocks a specific task
* \param iTaskNumber Task number to be blocked
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use block task function from interrupt handler code
*********************************************************************************************/
INT8U BlockTask(INT8U iTaskNumber);

/*****************************************************************************************//**
* \fn INT8U UnBlockTask(INT8U iTaskNumber)
* \brief UnBlocks a specific task
* \param iTaskNumber Task number to be unblocked
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use unblock task function from interrupt handler code
*********************************************************************************************/
INT8U UnBlockTask(INT8U iTaskNumber);

/*****************************************************************************************//**
* \fn INT8U BlockMultipleTask(INT8U TaskStart, INT8U TaskNumber)
* \brief Blocks a set of tasks
* \param TaskStart Number of the first task to be blocked
* \param TaskNumber Number of tasks to be blocked from the specified task start
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use block multiple tasks function from interrupt handler code
*********************************************************************************************/
INT8U BlockMultipleTask(INT8U TaskStart, INT8U TaskNumber);

/*****************************************************************************************//**
* \fn INT8U UnBlockMultipleTask(INT8U TaskStart, INT8U TaskNumber)
* \brief UnBlocks a set of tasks
* \param TaskStart Number of the first task to be unblocked
* \param TaskNumber Number of tasks to be unblocked from the specified task start
* \return OK - Success
* \return IRQ_PEND_ERR - Can not use unblock multiple tasks function from interrupt handler code
*********************************************************************************************/
INT8U UnBlockMultipleTask(INT8U TaskStart, INT8U TaskNumber);

/*********************************************************************************//**
* \fn void BRTOS_Init(void)
* \brief Initialize BRTOS control blocks and tick timer (Internal kernel function).
*************************************************************************************/
void BRTOS_Init(void);

/*****************************************************************//**
* \fn void OSSchedule(void)
* \brief BRTOS Scheduler function (Internal kernel function).
*********************************************************************/
void OSSchedule(void);

/*****************************************************************//**
* \fn INT8U SAScheduler(INT32U ReadyList)
* \brief Sucessive Aproximation Scheduler (Internal kernel function).
* \param ReadyList List of the tasks ready to run
* \return The priority of the highest priority task ready to run
*********************************************************************/
#if NUMBER_OF_PRIORITIES == 32
  INT8U SAScheduler(INT32U ReadyList);
#else
  INT8U SAScheduler(INT16U ReadyList);
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Variables Extern Declarations            /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if NUMBER_OF_PRIORITIES == 32
  extern INT32U OSReadyList;
  extern INT32U OSBlockedList;
#else
  extern INT16U OSReadyList;
  extern INT16U OSBlockedList;
#endif

#if NUMBER_OF_PRIORITIES == 32
extern const INT32U PriorityMask[32];
#else
extern const INT16U PriorityMask[16];
#endif 

extern ContextType *Tail;
extern ContextType *Head;

extern INT8U iNesting;
extern INT8U currentTask;
extern ContextType ContextTask[NUMBER_OF_TASKS + 2];
extern INT16U iStackAddress;
extern INT8U NumberOfInstalledTasks;
extern INT16U counter;
extern INT32U OSDuty;
extern INT8U PriorityVector[configMAX_TASK_INSTALL];
extern INT32U OSDutyTmp;
extern INT16U LastOSDuty;
extern INT8U STACK[HEAP_SIZE];
extern INT32U TaskAlloc;

extern INT8U  QUEUE_STACK[QUEUE_HEAP_SIZE];
extern INT16U iQueueAddress;

extern const CHAR8 *version;

#if (SP_SIZE == 32)
  extern INT32U StackAddress;
#endif

#if (SP_SIZE == 16)
  extern INT16U StackAddress;
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





#endif
