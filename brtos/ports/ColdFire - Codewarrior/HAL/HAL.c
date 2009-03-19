#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "BRTOS.h"


#ifdef __cplusplus
 extern "C"
#endif



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Tick Timer Setup                            /////
/////                                                  /////
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
  TPM1SC = 0x48;                       /* Int. flag clearing (2nd part) and timer control register setting */
  
  #if (Coldfire == 1)
    OSWaitLimit = TPM1MOD / 16;
    OSWaitLimit = TPM1MOD - OSWaitLimit;
  #endif
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
      OSDutyTmp = TPM1CNT;
      
      // Somente p/ Coldfire
      #if (Coldfire == 1)
      if (OSDutyTmp < OSWaitLimit)
        //_Wait;
        OS_Wait;
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

