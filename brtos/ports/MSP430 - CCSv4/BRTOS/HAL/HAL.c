/**
* \file HAL.c
* \brief BRTOS Hardware Abstraction Layer Functions.
*
* This file contain the functions that are processor dependant.
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
*                                   OS HAL Functions to MSP430
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "../includes/BRTOS.h"

INT8U flag_load = TRUE;


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Tick Timer Setup                         /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void TickTimerSetup(void)
{
    BCSCTL3 |= LFXT1S_2;  // VLO
	
	/* Ensure the timer is stopped. */
	TACTL = 0;

	/* Run the timer of the ACLK. */
	TACTL = TASSEL_1;

	/* Clear everything to start with. */
	TACTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TACCR0 = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) >> configTIMER_PRE_SCALER; /* Period value setting */   

	/* Enable the interrupts. */
	TACCTL0 = CCIE;

	/* Start up clean. */
	TACTL |= TACLR;

	/* Up mode. */
	TACTL |= MC_1;
  
  /* ### */
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS RTC Setup                                /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void OSRTCSetup(void)
{  
	// not used
	__asm ("nop");
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    OS Idle Task                                  /////
/////                                                  /////
/////    You must put the processor in standby mode    /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void Idle(void)
{
  /* task setup */
  
  /* task main loop */
  for (;;)
  {
     
     #if (DEBUG == 1)
       #if (COMPUTES_CPU_LOAD == 1)
       OSDutyTmp = TIMER_COUNTER;
       #endif
       OS_Wait;
     #else
       #if (COMPUTES_CPU_LOAD == 1)       
       if(flag_load == TRUE)
       {
           flag_load = FALSE;
           OSDutyTmp = TIMER_COUNTER;
       }
       OSExitCritical();
       #endif
     #endif

  }
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////  Task Installation Function                      /////
/////                                                  /////
/////  Parameters:                                     /////
/////  Function pointer, task priority and task name   /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U InstallTask(void(*FctPtr)(void),const CHAR8 *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority)
{
  INT8U i = 0; 
  INT8U TaskNumber = 0;
  ContextType * Task;
  
  INT16U NUMBER_OF_STACKED_BYTES = USER_STACKED_BYTES + NUMBER_MIN_OF_STACKED_BYTES;

   if ((iStackAddress + NUMBER_OF_STACKED_BYTES) > HEAP_SIZE)
   {
       return NO_MEMORY;
   }

   if (iPriority)
   {
     if (iPriority > configMAX_TASK_PRIORITY)
     {
        return END_OF_AVAILABLE_PRIORITIES;
     }
     
     if (PriorityVector[iPriority] != EMPTY_PRIO)
     {
        return BUSY_PRIORITY;
     }
   }
   else
   {
      return CANNOT_ASSIGN_IDLE_TASK_PRIO;
   }

   NumberOfInstalledTasks++;
   
   
   // Number Task Discovery
   for(i=0;i<NUMBER_OF_TASKS;i++)
   {
      INT16U teste = 1;
      teste = teste<<i;
    
      if (!(teste & TaskAlloc))
      { 
         TaskNumber = i+1;
         TaskAlloc = TaskAlloc | teste;
         break;
      }
   }   
   
   Task = &ContextTask[TaskNumber];
   
   Task->TaskName = TaskName;

   Task->StackPoint = StackAddress + USER_STACKED_BYTES;          // Posiciona o inicio do stack da tarefa
                                                                          // no inicio da disponibilidade de RAM do HEAP
   // Virtual Stack Init
   Task->StackInit = StackAddress + NUMBER_OF_STACKED_BYTES;

   // Determina a prioridade da função
   Task->Priority = iPriority;

   // Determina a tarefa que irá ocupar esta prioridade
   PriorityVector[iPriority] = TaskNumber;
   // set the function entry address in the context
   
   // First SR should be 0
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 4] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 3] = 0x00;
	  
   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned int) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned int) (FctPtr)) >> 8;   

	// Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x15;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x14;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 9] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 10] = 0x13;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 11] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 12] = 0x12;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 14] = 0x11;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 15] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 16] = 0x10;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 18] = 0x09;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 19] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 20] = 0x08;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 22] = 0x07;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 23] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 24] = 0x06;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 26] = 0x05;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 27] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 28] = 0x04;

   // Incrementa o contador de bytes do stack virtual (HEAP)
   iStackAddress = iStackAddress + NUMBER_OF_STACKED_BYTES;
   
   // Posiciona o endereço de stack virtual p/ a próxima tarefa instalada
   StackAddress = StackAddress + NUMBER_OF_STACKED_BYTES;
   

   Task->TimeToWait = NO_TIMEOUT;
   Task->Next     =  NULL;
   Task->Previous =  NULL;
   
   #if (VERBOSE == 1)
   Task->Blocked = FALSE;
   Task->State = READY;
   #endif   
   
   #if NUMBER_OF_PRIORITIES == 32
    OSReadyList = OSReadyList | ((INT32U)1 << iPriority);
   #else
    OSReadyList = OSReadyList | ((INT16U)1 << iPriority);
   #endif
   
   return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////  Idle Task Installation Function                 /////
/////                                                  /////
/////  Parameters:                                     /////
/////  Function pointer and task priority              /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES)
{ 
   INT16U NUMBER_OF_STACKED_BYTES = USER_STACKED_BYTES + NUMBER_MIN_OF_STACKED_BYTES; 

   if ((iStackAddress + NUMBER_OF_STACKED_BYTES) > HEAP_SIZE)
   {
       return NO_MEMORY;
   }

   ContextTask[NUMBER_OF_TASKS+1].StackPoint = StackAddress + USER_STACKED_BYTES;
   
   // Virtual Stack Init
   ContextTask[NUMBER_OF_TASKS+1].StackInit = StackAddress + NUMBER_OF_STACKED_BYTES;

   // Determina a prioridade da função
   ContextTask[NUMBER_OF_TASKS+1].Priority = 0;
   // Determina a tarefa que irá ocupar esta prioridade
   PriorityVector[0] = NUMBER_OF_TASKS+1;
   
   // set the function entry address in the context
     // First SR should be 0
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 4] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 3] = 0x00;
   
   // Pointer to Task Entry   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned int) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned int) (FctPtr)) >> 8;   
  
  	// Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x15;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x14;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 9] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 10] = 0x13;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 11] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 12] = 0x12;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 14] = 0x11;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 15] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 16] = 0x10;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 18] = 0x09;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 19] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 20] = 0x08;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 22] = 0x07;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 23] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 24] = 0x06;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 26] = 0x05;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 27] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 28] = 0x04;
  
   // Incrementa o contador de bytes do stack virtual (HEAP)
   iStackAddress = iStackAddress + NUMBER_OF_STACKED_BYTES;
   
   // Posiciona o endereço de stack virtual p/ a próxima tarefa instalada
   StackAddress = StackAddress + NUMBER_OF_STACKED_BYTES;
   
   
   ContextTask[NUMBER_OF_TASKS+1].TimeToWait = NO_TIMEOUT;
   
   #if (VERBOSE == 1)
   ContextTask[NUMBER_OF_TASKS+1].Blocked = FALSE;
   ContextTask[NUMBER_OF_TASKS+1].State = READY;  
   #endif
   
   #if NUMBER_OF_PRIORITIES == 32
    OSReadyList = OSReadyList | (INT32U)1;
   #else
    OSReadyList = OSReadyList | (INT16U)1;
   #endif
   
   return OK;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
		


