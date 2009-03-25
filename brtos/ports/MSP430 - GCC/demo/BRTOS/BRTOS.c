
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
*                                           Kernel functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "hardware.h"
#include "BRTOS.h"
#include "HAL.h"
#include "drivers.h"
#include "queue.h"
#include "OS_RTC.h"

wakeup interrupt(TIMERA0_VECTOR) TickTimer(void);
void SwitchContext(void);

const char *version={
  "BRTOS Ver. 0.9"
};

INT8U STACK[HEAP_SIZE];
INT8U PriorityVector[configMAX_TASK_INSTALL];
INT16U iStackAddress = 0;

#if (SP_SIZE == 32)
INT32U StackAddress = (INT32U)&STACK;
#endif

#if (SP_SIZE == 16)
INT16U StackAddress = (INT16U)&STACK;
#endif


ContextType ContextTask[NUMBER_OF_TASKS + 2]; // ContextTask[0] not used
                                              // Last ContexTask is the Idle Task

// global variables
// Task Manager Variables
INT8U NumberOfInstalledTasks,currentTask;
static INT8U TaskSelect = 0;

#if (SP_SIZE == 32)
  INT32U SPvalue;
  INT32U SPBackup = 0;
#endif

#if (SP_SIZE == 16)
  INT16U SPvalue;
  INT16U SPBackup = 0;
#endif

#if (Coldfire == 1)
  INT16U SRBackup;
#endif

INT16U counter;
static INT8U iLoop=0;
INT8U maxPriority = configMAX_TASK_INSTALL - 1;
INT16U OSDuty=0;
INT16U OSDutyTmp=0;
INT32U TaskAlloc = 0;
INT16U OSWaitLimit = 0;
INT8U iNesting = 0;





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Priority Preemptive Scheduler               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void OSSchedule(void)
{


	for (iLoop = maxPriority; iLoop < configMAX_TASK_INSTALL; iLoop++)
	{                          
	  if (PriorityVector[iLoop] != 0)
	  {
	    TaskSelect = PriorityVector[iLoop];
	    if (ContextTask[TaskSelect].Suspended == FALSE)
	    {
	      break;
	    }
	    else if (ContextTask[TaskSelect].Suspended == READY)
	    {
        ContextTask[TaskSelect].Suspended = FALSE;     // Tira tarefa do modo suspenso
	      break;  
	    }
	  }	  
	}
	
  if (currentTask != TaskSelect)
  {  
    ContextTask[currentTask].StackPoint = SPvalue;
    
    // FixSR somente p/ o Coldfire
    #if (Coldfire == 1)
    FixSR();
    #endif
  }
  
  if (currentTask != TaskSelect)
  { 
	  // Select the new Task
	  currentTask = TaskSelect;
	  
    // Select the new stack pointer value
    SPvalue = ContextTask[currentTask].StackPoint;    	    
  }
	
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Exit Interrupt Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Change context only if Nesting = 0
// i.e. if there are no interrupt nesting
void OS_INT_EXIT_CF(void)
{
  iNesting--;
  
  if (!iNesting)
      OSSchedule();
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Task Delay Function in Tick Times           /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Atraso em passos de TickCount
void DelayTask(INT16U time)
{
  INT16U time_wait = 0;
   
  OSEnterCritical;

  time_wait = counter + time;
  if (time_wait >= TickCountOverFlow)
    time_wait = time - (TickCountOverFlow - counter);
  
  ContextTask[currentTask].TimeToWait = time_wait;
  ContextTask[currentTask].Suspended = TRUE;
  ContextTask[currentTask].SuspendedType = DELAY;
  
  // Change context
  // Return to task when occur delay overflow
  ChangeContext();
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////   Task Delay Function in miliseconds, seconds,   /////
/////   minutes and hours                              /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Miliseconds, seconds, minutes and hours delay
INT8U DelayTaskHMSM(INT8U hours, INT8U minutes, INT8U seconds, INT16U miliseconds)
{
  INT32U ticks=0;
  INT16U loops=0;
  
  if (minutes > 59)
    return INVALID_TIME;
  
  if (seconds > 59)
    return INVALID_TIME;
  
  if (minutes > 999)
    return INVALID_TIME;  
  
  ticks = (INT32U)hours   * 3600L * configTICK_RATE_HZ
        + (INT32U)minutes * 60L   * configTICK_RATE_HZ
        + (INT32U)seconds *         configTICK_RATE_HZ
        + ((INT32U)miliseconds    * configTICK_RATE_HZ)/1000L;
  
  // Task Delay limit = 32000      
  loops = ticks / 30000L;
  ticks = ticks % 30000L;
  
  DelayTask(ticks);
  
  while(loops > 0)
  {
    DelayTask(30000);
    loops--;
  }
  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Tick Timer Function                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
   
wakeup interrupt(TIMERA0_VECTOR) TickTimer(void)
{
  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  // ************************  


  // Interrupt handling
  TACTL |= TACLR;

  //////////////////////////////////////////
  // System Load                          //
  //////////////////////////////////////////
  OSDuty = OSDutyTmp;
  OSDutyTmp = TA0R;
  //////////////////////////////////////////

  counter++;    
  if (counter == TickCountOverFlow) counter = 0;

  // Put task with delay overflow in the ready list
	for (iLoop = 1; iLoop <= NUMBER_OF_TASKS; iLoop++)
	{                          
	    if (ContextTask[iLoop].Suspended == TRUE)
	    { 
	        if (counter == ContextTask[iLoop].TimeToWait)
	        {	    	    
            ContextTask[iLoop].Suspended = READY;
	        }
	    }
	}

  // ************************
  // Interrupt Exit
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
  // There are an error in HCS08 C compiler
  // That error include two PULH at TickTimer interrupt exit
  // In order to correct this error, a PSHH instruction must be performed
  #if (HCS08 == 1)
  asm("PSHH");
  #endif
  
  
// ************************
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Init Task Scheduler Function             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U TaskStartScheduler(void)
{
  if (InstallIdle(&Idle,100) != OK)
  {
    return NO_MEMORY;
  }

  ChangeContext();
  
  // function must return something rev1.1
  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Function to Initialize RTOS Variables    /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void PreInstallTasks(void)
{
  int i=0;
  
  // rev1.1
  OSEnterCritical;
  
  counter = 0;
  currentTask = 0;
  NumberOfInstalledTasks = 0;
  TaskAlloc = 0;
  iStackAddress = 0;
  
  #if (Coldfire == 1)
    SRBackup = 0x2000;
  #endif
  
  for(i=0;i<configMAX_TASK_INSTALL;i++)
  {
    PriorityVector[i]=0;
  }
  
  #if (OSRTCEN == 1)
    OSRTCSetup();
  #endif
  
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////   Software Interrupt to provide Switch Context   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void SwitchContext(void)

{
  // as MSP430 does not have sw interrupt, we save 7 regs to make it appear like one.
  // save 7 regs
  OS_SAVE_ISR();
  
  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();

  
  // Interrupt Handling 
  iNesting--;
  
  if (iNesting == 0)
    OSSchedule();
  
  
  OS_RESTORE_CONTEXT();
  OS_RESTORE_ISR();
   
  
  OSExitCritical;
 

}
 
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


