#ifndef OS_MYRTOS_H
#define OS_MYRTOS_H

#include "OS_types.h"
#include "HAL.h"

// Define if simulation or DEBUG
#define DEBUG 1

// Define if whatchdog active
#define COP 1

// Define CPU Stack Pointer Size
#define SP_SIZE 32

// Define the number of Task to be Installed
// must always be equal or higher to NumberOfInstalledTasks
#define NUMBER_OF_TASKS 6


#define FALSE (INT8U)0
#define TRUE  (INT8U)1
#define READY (INT8U)2

#define OK                           (INT8U)0
#define NO_MEMORY                    (INT8U)1
#define END_OF_AVAILABLE_PRIORITIES  (INT8U)2
#define BUSY_PRIORITY                (INT8U)3
#define INVALID_TIME                 (INT8U)4


// Suspended Types
#define DELAY     0
#define SEMAPHORE 1
#define MAILBOX   2
#define QUEUE     3
#define MUTEX     4


// TickTimer Defines
#define configCPU_CLOCK_HZ          (INT32U)24008000
#define configTICK_RATE_HZ          (INT32U)1000
#define configTIMER_PRE_SCALER      0
#define TickCountOverFlow           (INT16U)32000
#define configRTC_CRISTAL_HZ        (INT32U)1000
#define configRTC_PRE_SCALER        10
#define OSRTCEN                     0


//Stack Defines
// P/ Coldfire com 8KB de RAM, limite de 5KB p/ STACK Virtual
#define HEAP_SIZE 5*1024


// **********************************************************
// **********************************************************

// Others Defines
#define configMAX_TASK_NAME_LEN 16
#define configMAX_TASK_INSTALL 32


typedef struct ContextTag                                                 
{
   //INT8S Name[configMAX_TASK_NAME_LEN];
   INT16U StackPoint;
   INT16U StackInit;
   INT16U TimeToWait;
   INT8U  Suspended;        // Substituir por state - running, suspended, etc
   INT8U  SuspendedType;
   INT8U  Priority;         // adicionar funcionalidades
} ContextType;



// Keyboard Defines
#define KeybPort        PTDD
#define KeybTimerDelay  10          // In TickCounter times
#define ActiveKeyb      TRUE        // Keyb Activated


// Functions Prototypes

INT8U  TaskStartScheduler(void);
void DelayTask(INT16U time);
INT8U DelayTaskHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U miliseconds);
void OS_INT_EXIT_CF(void);
void PreInstallTasks(void);




extern INT8U iNesting;
extern INT8U currentTask;
extern ContextType ContextTask[NUMBER_OF_TASKS + 2];
extern INT16U iStackAddress;
extern INT8U NumberOfInstalledTasks;
extern INT16U counter;
extern INT16U OSDuty;
extern INT8U PriorityVector[configMAX_TASK_INSTALL];
extern INT16U OSDutyTmp;
extern INT16U OSWaitLimit;
extern INT8U STACK[HEAP_SIZE];
extern INT8U maxPriority;
extern INT32U TaskAlloc;
extern const char *version;

#if (SP_SIZE == 32)
  extern INT32U StackAddress;
#endif

#if (SP_SIZE == 16)
  extern INT16U StackAddress;
#endif


#endif
