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
*                                   OS HAL Functions to Coldfire V1
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "hardware.h"
#include "BRTOS.h"

#pragma warn_implicitconv off
#pragma warn_unusedarg off

volatile INT8U flag_load = TRUE;


#ifdef __cplusplus
 extern "C"
#endif





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Tick Timer Setup                         /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void TickTimerSetup(void)
{
  
  /* ### Init_TPM init code */
  /* TPM1SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  TPM1SC = 0x00;                       /* Stop and reset counter */
  TPM1MOD = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) >> configTIMER_PRE_SCALER; /* Period value setting */
  (void)(TPM1SC == 0);                 /* Overflow int. flag clearing (first part) */
  /* TPM1SC: TOF=0,TOIE=1,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
  TPM1SC = 0x48 | configTIMER_PRE_SCALER;                       /* Int. flag clearing (2nd part) and timer control register setting */
  
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
  /* ### Init_RTC init code */
  /* RTCMOD: RTCMOD=0x63 */
  // Cristal de Referência = 1Khz
  RTCMOD = (configRTC_CRISTAL_HZ / configRTC_PRE_SCALER);
  /* RTCSC: RTIF=1,RTCLKS=0,RTIE=1,RTCPS=0x0B */
  RTCSC = 0x1B;                                      
  /* ### */

  //OSResetTime(&Hora);
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
      INT32U teste = 1;
      teste = teste<<i;
    
      if (!(teste & TaskAlloc))
      { 
         TaskNumber = i+1;
         TaskAlloc = TaskAlloc | teste;
         break;
      }
   }   
   
   Task = &ContextTask[TaskNumber];
   
   //strncpy( ( char * ) ContextTask[TaskNumber].Name, ( const char * ) TaskName, configMAX_TASK_NAME_LEN );
   
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
   
   // First 4 bytes defined to Coldfire Only
   // Format: First 4 bits = processor indicating a two-longword frame, always 0x04 in MCF51QE
   //         Other 4 bits = fault status field, always 0x00 if no error occurred
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x40;
   // Vector
   // The 8-bit vector number, vector[7:0], defines the exception type and is
   // calculated by the processor for all internal faults and represents the
   // value supplied by the interrupt controller in the case of an interrupt
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x80;
   
   // Initial SR Register
   // Interrupts Enabled
   // CCR = 0x00
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x20;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;

   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned long) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned long) (FctPtr)) >> 8;  
   
   #if (NESTING_INT == 1)  
   
   // Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0xA1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0xA0;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0xD2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0xD1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 29] = 0xD0;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 33] = 0xA6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 37] = 0xA5;   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 41] = 0xA4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 45] = 0xA3;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 49] = 0xA2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 53] = 0xD7;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 57] = 0xD6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 61] = 0xD5;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 65] = 0xD4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 69] = 0xD3;
   
   #else
   
   // Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 9] = 0xA1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0xA0;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0xD2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0xD1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0xD0;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 29] = 0xA6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 33] = 0xA5;   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 37] = 0xA4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 41] = 0xA3;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 45] = 0xA2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 49] = 0xD7;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 53] = 0xD6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 57] = 0xD5;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 61] = 0xD4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 65] = 0xD3;
   #endif
   
   
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
   
   OSReadyList = OSReadyList | (PriorityMask[iPriority]);
   
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
   // First 4 bytes defined to Coldfire Only
   
   // Format: First 4 bits = processor indicating a two-longword frame, always 0x04 in MCF51QE
   //         Other 4 bits = fault status field, always 0x00 if no error occurred   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x40;
   
   // Vector
   // The 8-bit vector number, vector[7:0], defines the exception type and is
   // calculated by the processor for all internal faults and represents the
   // value supplied by the interrupt controller in the case of an interrupt
   // To CFV1, practical value are 0x80 to RAM
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x80;   
   
   // Initial SR Register
   // Interrupts Enabled
   // CCR = 0x00   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x20;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;
   
   // Pointer to Task Entry   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned long) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned long) (FctPtr)) >> 8;   
   
   
   #if (NESTING_INT == 1)  
   
   // Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0xA1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0xA0;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0xD2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0xD1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 29] = 0xD0;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 33] = 0xA6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 37] = 0xA5;   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 41] = 0xA4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 45] = 0xA3;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 49] = 0xA2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 53] = 0xD7;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 57] = 0xD6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 61] = 0xD5;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 65] = 0xD4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 69] = 0xD3;
   
   #else
   
   // Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 9] = 0xA1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0xA0;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0xD2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0xD1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0xD0;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 29] = 0xA6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 33] = 0xA5;   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 37] = 0xA4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 41] = 0xA3;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 45] = 0xA2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 49] = 0xD7;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 53] = 0xD6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 57] = 0xD5;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 61] = 0xD4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 65] = 0xD3;
   #endif  
   
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


#if (NESTING_INT == 1)

INT16U OS_CPU_SR_Save(void)
{  
  asm
  {
    
        MOVE.W   SR,D0            // Copy SR into D0 
        MOVE.L   D0,-(A7)         // Save D0
        ORI.L    #0x0700,D0       // Disable interrupts
        MOVE.W   D0,SR
        MOVE.L   (A7)+,D0         // Restore D0
  }
}


void OS_CPU_SR_Restore(INT16U)
{  
  asm
  {
    
        MOVE.L   D0,-(A7)         // Save D0
        MOVE.W   2(A7),D0
        MOVE.W    D0,SR
        MOVE.L   (A7)+,D0         // Restore D0
  }
}

  
  
void CriticalDecNesting(void)
{  
  asm                                                                   
  {                                                                     
        MOVE.W   SR,D2
        ORI.L    #0x0700,D2
        MOVE.W   D2,SR
        MVZ.B    iNesting,D0
        SUBQ.L   #1,D0
        MOVE.B   D0,iNesting                                                  
  }  
}

#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
