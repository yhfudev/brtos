/**
* \file BRTOS.c
* \brief BRTOS kernel functions
*
* Kernel functions, such as: scheduler, block tasks, unblock tasks, Delay, Change Context
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
*                                           Kernel functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.1
*   Date:     11/03/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.2
*   Date:     01/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.3
*   Date:     11/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.4
*   Date:     19/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.45
*   Date:     20/10/2010
*
*********************************************************************************************************/

#include "includes/BRTOS.h"
#include "includes/drivers.h"
#include "includes/OS_RTC.h"
#include "HAL/HAL.h"

void SwitchContext(void);

const CHAR8 *version=                            ///< Informs BRTOS version
{
  "BRTOS Ver. 1.4"
};

INT8U STACK[HEAP_SIZE];                         ///< Virtual Task stack
INT8U PriorityVector[configMAX_TASK_INSTALL];   ///< Allocate task priorities
INT16U iStackAddress = 0;                       ///< Virtual stack counter - Informs the stack occupation in bytes

INT8U QUEUE_STACK[QUEUE_HEAP_SIZE];             ///< Queue heap
INT16U iQueueAddress = 0;                       ///< Queue heap control


#if (SP_SIZE == 16)
INT16U StackAddress = (INT16U)&STACK;           ///< Virtual stack pointer
#endif


ContextType ContextTask[NUMBER_OF_TASKS + 2]; ///< Task context info
                                              ///< ContextTask[0] not used
                                              ///< Last ContexTask is the Idle Task

// global variables
// Task Manager Variables
INT8U NumberOfInstalledTasks;                 ///< Number of Installed tasks at the moment
INT8U currentTask;                            ///< Current task being executed

#if (SP_SIZE == 16)
//INT16U SPvalue;                             ///< Used to save and restore a task stack pointer
#endif


#if NUMBER_OF_PRIORITIES == 32
  INT32U OSReadyList = 0;
  INT32U OSBlockedList = 0xFFFFFFFF;
#else
  INT16U OSReadyList = 0;
  INT16U OSBlockedList = 0xFFFF;
#endif

INT16U counter;                                   ///< Incremented each tick timer - Used in delay and timeout functions
INT32U OSDuty=0;                                  ///< Used to compute the CPU load
INT32U OSDutyTmp=0;                               ///< Used to compute the CPU load
INT16U LastOSDuty = 0;                            ///< Last CPU load computed
INT16U DutyCnt = 0;                               ///< Used to compute the CPU load
INT32U TaskAlloc = 0;                             ///< Used to search a empty task control block
INT8U  iNesting = 0;                              ///< Used to inform if the current code position is an interrupt handler code

ContextType *Tail;
ContextType *Head;


#if NUMBER_OF_PRIORITIES == 32
const INT32U PriorityMask[32]=
{
  0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000,
  0x010000,0x020000,0x040000,0x080000,0x100000,0x200000,0x400000,0x800000,0x01000000,0x02000000,
  0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000
};
#else
const INT16U PriorityMask[16]=
{
  0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000
};
#endif 


void OSTimerWakeUp(void);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Priority Preemptive Scheduler               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/************************************************************//**
* \fn void OSSchedule(void)
* \brief Priority Preemptive Scheduler (Internal kernel function).
****************************************************************/

void OSSchedule(void)
{
	INT8U TaskSelect;
	INT8U Priority;
	
  Priority = SAScheduler(OSReadyList & OSBlockedList);
  TaskSelect = PriorityVector[Priority];
  
  if (currentTask != TaskSelect)
  {  
    
    ContextTask[currentTask].StackPoint = SPvalue;

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
/////      Task Delay Function in Tick Times           /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Atraso em passos de TickCount
INT8U DelayTask(INT16U time_wait)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  ContextType *Task = &ContextTask[currentTask];
   
  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be blocked by interrupt
  }

  if (currentTask)
  {
    OSEnterCritical();
    
    // BRTOS TRACE SUPPORT
    #if (OSTRACE == 1) 
        #if(OS_TRACE_BY_TASK == 1)
        Update_OSTrace(currentTask, DELAYTASK);
        #else
        Update_OSTrace(Task->Priority, DELAYTASK);
        #endif
    #endif    

    time_wait = counter + time_wait;
    if (time_wait >= TickCountOverFlow)
      time_wait = time_wait - TickCountOverFlow;
    
    Task->TimeToWait = time_wait;
    
    // Put task into delay list
    if(Tail != NULL)
    { 
      // Insert task into list
      Tail->Next = Task;
      Task->Previous = Tail;
      Tail = Task;
      Tail->Next = NULL;
    }
    else{
       // Init delay list
       Tail = Task;
       Head = Task; 
    }    
    
    #if (VERBOSE == 1)
    Task->State = SUSPENDED;
    Task->SuspendedType = DELAY;
    #endif
    
    OSReadyList = OSReadyList & ~(PriorityMask[Task->Priority]);
    
    // Change context
    // Return to task when occur delay overflow
    ChangeContext();
    
    OSExitCritical();
    
    return OK;
  }
  else
  {
    return NOT_VALID_TASK;
  }
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
  INT32U loops=0;
  
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
  
  (void)DelayTask(ticks);
  
  while(loops > 0)
  {
    (void)DelayTask(30000);
    loops--;
  }
  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





void OSTimerWakeUp(void)
{  
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U  iPrio = 0;  
  ContextType *Task = Head;
  
  while(Task != NULL)
  {      
      if (Task->TimeToWait == counter)
      {

        iPrio = Task->Priority;
        
        #if (NESTING_INT == 1)
        OSEnterCritical();
        #endif        

        if((OSReadyList & PriorityMask[iPrio]) == FALSE)
        {
            #if (VERBOSE == 1)
            Task->State = READY;
            #endif
            Task->TimeToWait = EXIT_BY_TIMEOUT;

            OSReadyList = OSReadyList | (PriorityMask[iPrio]);
        }
        
        #if (NESTING_INT == 1)
        OSExitCritical();
        #endif                  
          
        // Remove from delay list
        if(Task == Head)
        {
          Head = Task->Next;
          Head->Previous = NULL;
          if(Task == Tail)
          {
            Tail = Task->Previous;
            Tail->Next = NULL;
          }          
        }
        else
        {          
          if(Task == Tail)
          {
            Tail = Task->Previous;
            Tail->Next = NULL;
          }
          else
          {
            Task->Next->Previous = Task->Previous;
            Task->Previous->Next = Task->Next; 
          }
        }
      }
 
      Task = Task->Next;    
	}
}




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Init Task Scheduler Function             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U TaskStartScheduler(void)
{
  if (InstallIdle(&Idle,IDLE_STACK_SIZE) != OK)
  {
    return NO_MEMORY;
  };

  OSExitCritical();
  ChangeContext();
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
  INT8U i=0;
  OSEnterCritical();
  counter = 0;
  currentTask = 0;
  NumberOfInstalledTasks = 0;
  TaskAlloc = 0;
  iStackAddress = 0;
  
  for(i=0;i<configMAX_TASK_INSTALL;i++)
  {
    PriorityVector[i]=EMPTY_PRIO;
  }
    
  Tail = NULL;
  Head = NULL;
  
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
/////      Block Priority Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U BlockPriority(INT8U iPriority)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif  
  INT8U BlockedTask = 0;
  
  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be blocked by interrupt
  }
      
  // Enter critical Section
  if (currentTask)  
    OSEnterCritical();


  // Detects the task priority
  BlockedTask = PriorityVector[iPriority];  
  // Block task with priority iPriority
  #if (VERBOSE == 1)
  ContextTask[BlockedTask].Blocked = TRUE;
  #endif
  
  OSBlockedList = OSBlockedList & ~(PriorityMask[iPriority]);
   
  
  if (currentTask == BlockedTask)
  {
     ChangeContext();
  }

  // Exit critical Section
  if (currentTask)
    OSExitCritical();
  
  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      UnBlock Priority Function                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U UnBlockPriority(INT8U iPriority)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  #if (VERBOSE == 1)
  INT8U BlockedTask = 0;
  #endif
  
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
    
  // Detects the task priority
  #if (VERBOSE == 1)  
  BlockedTask = PriorityVector[iPriority];  
  ContextTask[BlockedTask].Blocked = FALSE;
  #endif
  
  OSBlockedList = OSBlockedList | (PriorityMask[iPriority]);
  
  // check if we have unblocked a higher priority task  
  if (currentTask)
  {
    if (!iNesting)
    {
       ChangeContext();
    }
  }
  
  // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSExitCritical();

  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Block Task Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U BlockTask(INT8U iTaskNumber)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iPriority = 0;
  
  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be blocked by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
    OSEnterCritical();    

  // Determina a prioridade da função
  #if (VERBOSE == 1)
  ContextTask[iTaskNumber].Blocked = TRUE;
  #endif
  iPriority = ContextTask[iTaskNumber].Priority;
  
  OSBlockedList = OSBlockedList & ~(PriorityMask[iPriority]);
  
  if (currentTask == iTaskNumber)
  {
     ChangeContext();     
  }
  
  // Exit critical Section
  if (currentTask)
    OSExitCritical();  

  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      UnBlock Task Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U UnBlockTask(INT8U iTaskNumber)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iPriority = 0;
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();

  #if (VERBOSE == 1)
  ContextTask[iTaskNumber].Blocked = FALSE;
  #endif
  
  // Determina a prioridade da função  
  iPriority = ContextTask[iTaskNumber].Priority;

  OSBlockedList = OSBlockedList | (PriorityMask[iPriority]);
  
  // check if we have unblocked a higher priority task  
  if (currentTask)
  {
    if (!iNesting)
    {
       ChangeContext();
    }
  }
  
  // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSExitCritical();

  return OK;  
  
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Block Multiple Task Function                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U BlockMultipleTask(INT8U TaskStart, INT8U TaskNumber)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iTask = 0;
  INT8U TaskFinish = 0;
  INT8U iPriority = 0;  
  
  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be blocked by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
    OSEnterCritical();
  
  TaskFinish = (INT8U)(TaskStart + TaskNumber);
  
  for (iTask = TaskStart; iTask <TaskFinish; iTask++)
  {
    if (iTask != currentTask)
    {      
      #if (VERBOSE == 1)
      ContextTask[iTask].Blocked = TRUE;
      #endif
      // Determina a prioridade da função
      iPriority = ContextTask[iTask].Priority;   
      
      OSBlockedList = OSBlockedList & ~(PriorityMask[iPriority]);
    }
  }
  
  // Exit critical Section
  if (currentTask)
    OSExitCritical();

  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      unBlock Multiple Task Function              /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U UnBlockMultipleTask(INT8U TaskStart, INT8U TaskNumber)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iTask = 0;
  INT8U TaskFinish = 0;
  INT8U iPriority = 0;    
  
  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be blocked by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
    OSEnterCritical();
  
  TaskFinish = (INT8U)(TaskStart + TaskNumber);
  
  for (iTask = TaskStart; iTask <TaskFinish; iTask++)
  {
    // Determina a prioridade da função
    if (iTask != currentTask)
    {
      iPriority = ContextTask[iTask].Priority;
      
      #if (VERBOSE == 1)
      ContextTask[iTask].Blocked = FALSE;
      #endif
      
      OSBlockedList = OSBlockedList | (PriorityMask[iPriority]);
    }
  }
  
  // Exit critical Section
  if (currentTask)
    OSExitCritical();

  return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




void BRTOS_Init(void)
{  
  ////////////////////////////////////////////////////////////  
  /////      Initialize Event Control Blocks             /////
  ////////////////////////////////////////////////////////////
  initEvents();
  
  ////////////////////////////////////////////////////////////  
  /////          Initialize global variables             /////
  ////////////////////////////////////////////////////////////  
  PreInstallTasks();  
  
  ////////////////////////////////////////////////////////////  
  /////            Initialize Tick Timer                 /////
  ////////////////////////////////////////////////////////////  
  TickTimerSetup(); 
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Sucessive Aproximation Scheduler Algorithm    /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#if NUMBER_OF_PRIORITIES == 32
INT8U SAScheduler(INT32U ReadyList)
#else
INT8U SAScheduler(INT16U ReadyList)
#endif
{
  INT8U prio = 0;
  
  #if NUMBER_OF_PRIORITIES == 32
  
  if (ReadyList > 0xFFFF)
  {
    if (ReadyList > 0xFFFFFF)
    {
      if (ReadyList > 0xFFFFFFF)
      {        
        if (ReadyList > 0x3FFFFFFF)
        {
          if (ReadyList > 0x7FFFFFFF)
          {
            prio = 31;
          }
          else
          {
            prio = 30;
          }
        }
        else
        {
          if (ReadyList > 0x1FFFFFFF)
          {
            prio = 29; 
          }
          else
          {
            prio = 28;
          }
        }
      }
      else
      {
        if (ReadyList > 0x3FFFFFF)
        {
          if (ReadyList > 0x7FFFFFF)
          {
            prio = 27;
          }
          else
          {
            prio = 26;
          }
        }
        else
        {
          if (ReadyList > 0x1FFFFFF)
          {
            prio = 25;
          }
          else
          {
            prio = 24;
          }
        }
      }    
    }
    else
    {
      if (ReadyList > 0xFFFFF)
      {
        if (ReadyList > 0x3FFFFF)
        {
          if (ReadyList > 0x7FFFFF)
          {
            prio = 23;
          }
          else
          {
            prio = 22;
          }
        }
        else
        {
          if (ReadyList > 0x1FFFFF)
          {
            prio = 21; 
          }
          else
          {
            prio = 20;
          }
        }
      }
      else
      {
        if (ReadyList > 0x3FFFF)
        {
          if (ReadyList > 0x7FFFF)
          {
            prio = 19;
          }
          else
          {
            prio = 18;
          }
        }
        else
        {
          if (ReadyList > 0x1FFFF)
          {
            prio = 17;
          }
          else
          {
            prio = 16;
          }
        }
      }
    }  
  }
  else
  {
  #endif
    if (ReadyList > 0xFF)
    {
      if (ReadyList > 0xFFF)
      {        
        if (ReadyList > 0x3FFF)
        {
          if (ReadyList > 0x7FFF)
          {
            prio = 15;
          }
          else
          {
            prio = 14;
          }
        }
        else
        {
          if (ReadyList > 0x1FFF)
          {
            prio = 13; 
          }
          else
          {
            prio = 12;
          }
        }
      }
      else
      {
        if (ReadyList > 0x3FF)
        {
          if (ReadyList > 0x7FF)
          {
            prio = 11;
          }
          else
          {
            prio = 10;
          }
        }
        else
        {
          if (ReadyList > 0x1FF)
          {
            prio = 9;
          }
          else
          {
            prio = 8;
          }
        }
      }    
    }
    else
    {
      if (ReadyList > 0x0F)
      {
        if (ReadyList > 0x3F)
        {
          if (ReadyList > 0x7F)
          {
            prio = 7;
          }
          else
          {
            prio = 6;
          }
        }
        else
        {
          if (ReadyList > 0x1F)
          {
            prio = 5; 
          }
          else
          {
            prio = 4;
          }
        }
      }
      else
      {
        if (ReadyList > 0x03)
        {
          if (ReadyList > 0x07)
          {
            prio = 3;
          }
          else
          {
            prio = 2;
          }
        }
        else
        {
          if (ReadyList > 0x1)
          {
            prio = 1;
          }
          else
          {
            prio = 0;
          }
        }
      }
    }
  #if NUMBER_OF_PRIORITIES == 32
  }
  #endif
  return prio;
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////   Software Interrupt to provide Switch Context   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
 
/************************************************************//**
* \fn fake interrupt void SwitchContext(void)
* \brief Software interrupt handler routine (Internal kernel function).
*  Used to switch the tasks context.
****************************************************************/

void SwitchContext(void)
{
  
  // as MSP430 does not have sw interrupt, we save 7 regs to make it appear like one.
  // save 7 regs
  OS_SAVE_ISR();
  
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();
  
  // Interrupt Handling  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT(); 

  // ************************
  OS_RESTORE_ISR();
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

/************************************************************//**
* \fn interrupt void TickTimer(void)
* \brief Tick timer interrupt handler routine (Internal kernel function).
****************************************************************/

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void TickTimer (void)
{

  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER(); 

  // Interrupt handling
  TICKTIMER_INT_HANDLER;

  counter++;
  if (counter == TickCountOverFlow) counter = 0;
  
  
  // BRTOS TRACE SUPPORT
  #if (OSTRACE == 1) 
      #if(OS_TICK_SHOW == 1) 
          #if(OS_TRACE_BY_TASK == 1)
          Update_OSTrace(0, ISR_TICK);
          #else
          Update_OSTrace(configMAX_TASK_INSTALL - 1, ISR_TICK);
          #endif         
      #endif       
  #endif  

  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif  
  
  ////////////////////////////////////////////////////
  // Put task with delay overflow in the ready list //
  ////////////////////////////////////////////////////
  OSTimerWakeUp();
  
  #if (COMPUTES_CPU_LOAD == 1)
 	#if(DEBUG == 1)
     if (DutyCnt >= 1024)
     {
       DutyCnt = 0;
       OSDuty = (INT32U)((INT32U)OSDuty + (INT32U)OSDutyTmp);
       LastOSDuty = (INT16U)(OSDuty >> 10);
       OSDuty = 0;
     }else
     {    
       OSDuty = (INT32U)((INT32U)OSDuty + (INT32U)OSDutyTmp);
       DutyCnt++;
     }
     OSDutyTmp = TIMER_MODULE;   
   	#else   
     extern INT8U flag_load;
     
     flag_load = TRUE;
     
     if (DutyCnt >= 128)
     {
       DutyCnt = 0;
       OSDuty = (INT32U)((INT32U)OSDuty + (INT32U)OSDutyTmp);
       LastOSDuty = OSDuty >> 7;
       OSDuty = 0;
     }else
     {    
       OSDuty = (INT32U)((INT32U)OSDuty + (INT32U)OSDutyTmp);    
       DutyCnt++;
     }
     OSDutyTmp = TIMER_MODULE;
     #endif
  #endif
  //////////////////////////////////////////  

  // ************************
  // Interrupt Exit
  // ************************
   OS_INT_EXIT();
  // ************************
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




