
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
*                                           MSP430 port functions - MSPGCC compiler
*
*
*   Author:   Carlos H. Barriquello
*   Revision: 1.0
*   Date:     24/03/2009
*
*********************************************************************************************************/

#include "hardware.h"
#include "BRTOS.h"




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Tick Timer Setup                            /////
/////                                                  /////
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
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void OSRTCSetup(void)
{  
	// not used
	nop();
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////    Idle Task                                     /////
/////                                                  /////
/////    You must put the processor in standby mode    /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void Idle(void){
   /* task setup */

  
   /* task main loop */
   for (;;)
   {
		//enter low power mode
      __bis_SR_register(CPUOFF + GIE);
	  //nop();
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
INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority){

  INT8U i = 0;
  INT8U TaskNumber = 0;
  
  INT16U NUMBER_OF_STACKED_BYTES = USER_STACKED_BYTES + NUMBER_MIN_OF_STACKED_BYTES;

   if ((iStackAddress + NUMBER_OF_STACKED_BYTES) > HEAP_SIZE)
   {
       return NO_MEMORY;
   }
   
   if (iPriority > (configMAX_TASK_INSTALL-2))
   {
      return END_OF_AVAILABLE_PRIORITIES;
   }
   
   if (PriorityVector[iPriority] != 0)
   {
      return BUSY_PRIORITY;
   }

   NumberOfInstalledTasks++;
   
   
   // Number Task Discovery
   for(i=0;i<NUMBER_OF_TASKS;i++)
   {
      INT32U teste = 1;
      teste = teste<<i;
    
      if (!(teste & TaskAlloc))
      { 
         TaskNumber = i+1;
         TaskAlloc = TaskAlloc | teste;
         break;
      }
   }   
   
   //strncpy( ( char * ) ContextTask[TaskNumber].Name, ( const char * ) TaskName, configMAX_TASK_NAME_LEN );

   ContextTask[TaskNumber].StackPoint = StackAddress + USER_STACKED_BYTES;          // Posiciona o inicio do stack da tarefa
                                                                          // no inicio da disponibilidade de RAM do HEAP
   // Virtual Stack Init
   ContextTask[TaskNumber].StackInit = StackAddress + NUMBER_OF_STACKED_BYTES;

   // Determina a prioridade da função
   ContextTask[TaskNumber].Priority = iPriority;
   if (iPriority < maxPriority)
      maxPriority = iPriority;
   // Determina a tarefa que irá ocupar esta prioridade
   PriorityVector[iPriority] = TaskNumber;
   // set the function entry address in the context
   
      
  // First SR should be 0
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 4] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 3] = 0x00;
	  
   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned int) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned int) (FctPtr)) >> 8;   
   
   // Incrementa o contador de bytes do stack virtual (HEAP)
   iStackAddress = iStackAddress + NUMBER_OF_STACKED_BYTES;
   
   // Posiciona o endereço de stack virtual p/ a próxima tarefa instalada
   StackAddress = StackAddress + NUMBER_OF_STACKED_BYTES;
   

   ContextTask[TaskNumber].TimeToWait = 0;
   ContextTask[TaskNumber].Suspended = FALSE;
   
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
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES){
  
   INT16U NUMBER_OF_STACKED_BYTES = USER_STACKED_BYTES + NUMBER_MIN_OF_STACKED_BYTES; 

   if ((iStackAddress + NUMBER_OF_STACKED_BYTES) > HEAP_SIZE)
   {
       return NO_MEMORY;
   }

   ContextTask[NUMBER_OF_TASKS+1].StackPoint = StackAddress + USER_STACKED_BYTES;
   
   // Virtual Stack Init
   ContextTask[NUMBER_OF_TASKS+1].StackInit = StackAddress + NUMBER_OF_STACKED_BYTES;

   // Determina a prioridade da função
   ContextTask[NUMBER_OF_TASKS+1].Priority = configMAX_TASK_INSTALL-1;
   // Determina a tarefa que irá ocupar esta prioridade
   PriorityVector[configMAX_TASK_INSTALL-1] = NUMBER_OF_TASKS+1;
   
     // First SR should be 0
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 4] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 3] = 0x00;
   
   // Pointer to Task Entry   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned int) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned int) (FctPtr)) >> 8;   
   
   // Incrementa o contador de bytes do stack virtual (HEAP)
   iStackAddress = iStackAddress + NUMBER_OF_STACKED_BYTES;
   
   // Posiciona o endereço de stack virtual p/ a próxima tarefa instalada
   StackAddress = StackAddress + NUMBER_OF_STACKED_BYTES;
   
   ContextTask[NUMBER_OF_TASKS+1].Suspended = FALSE;
   
   return OK;
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

