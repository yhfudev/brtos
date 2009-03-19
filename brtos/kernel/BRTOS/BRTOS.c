#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "BRTOS.h"
#include "HAL.h"
#include "drivers.h"
#include "queue.h"
#include "OS_RTC.h"


#ifdef __cplusplus
 extern "C"
#endif

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
	
	
  /* backup current task context stored on stack if no more weight */
  // CurrentWeight deve ser menor ou igual a 1 para evitar que uma interrupção decremente
  // CurrentWeight antes da tarefa estar completa. Desta forma, quando a função delay for chamada
  // colocará CurrentWeight p/ um valor negativo
  //if ((currentTask != TaskSelect) && (currentWeight <= 0))
  if (currentTask != TaskSelect)
  {  
    ContextTask[currentTask].StackPoint = SPvalue;
    
    // FixSR somente p/ o Coldfire
    #if (Coldfire == 1)
    FixSR();
    #endif
  }
  
    
  /* increment task if no more weight (or if task == 0) */
  if (currentTask != TaskSelect)
  { 
	  currentTask = TaskSelect;
	  
    /* set RTI ISR stack to the stackdepth of the task */
    SPvalue = ContextTask[currentTask].StackPoint;    	    
  }
	
}






// Analisa no Stack se ocorreu outra interrupção antes do bloqueio
void OS_INT_EXIT_CF(void)
{
  iNesting--;
  
  if (!iNesting)
      OSSchedule();
}



// Atraso em passos de TickCount
void DelayTask(INT16U time)
{
  INT16U time_wait = 0;

  // Bloqueia as interrupções devido
  // ao código não ser reentrante
  // depende da variável global counter
   
  OSEnterCritical;

  time_wait = counter + time;
  if (time_wait >= TickCountOverFlow)
    time_wait = time - (TickCountOverFlow - counter);
  
  ContextTask[currentTask].TimeToWait = time_wait;
  ContextTask[currentTask].Suspended = TRUE;
  ContextTask[currentTask].SuspendedType = DELAY;
  
  // Troca contexto e só retorna quando estourar Delay
  ChangeContext();
}






// Atraso em milisegundos, segundos, minutos e horas
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
        
  
  // O limite da função Delay Task é 32000
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



/*
** ===================================================================
**     Interrupt handler : TickTimer
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
interrupt void TickTimer(void)
{
  /* Write your interrupt code here ... */

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  // ************************  

  // Tratamento da interrupção
  
  TPM1SC_TOF = 0;

  //////////////////////////////////////////
  // System Load                          //
  //////////////////////////////////////////
  OSDuty = OSDutyTmp;
  OSDutyTmp = TPM1MOD;
  //////////////////////////////////////////

  counter++;    
  if (counter == TickCountOverFlow) counter = 0;

  // Coloca as tarefas com estouro de delay na ready list
	for (iLoop = 1; iLoop <= NUMBER_OF_TASKS; iLoop++)
	{                          
	    if (ContextTask[iLoop].Suspended == TRUE)
	    { 
	        if (counter == ContextTask[iLoop].TimeToWait)
	        {	    	    
            ContextTask[iLoop].Suspended = READY;     // Coloca a tarefa na Ready List
	        }
	    }
	}

  // ************************
  // Saída de interrupção
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
/* end of TickTimer */






/* task manager functions  **********************************/
// Inicia o agendamento de tarefas
INT8U TaskStartScheduler(void)
{
  if (InstallIdle(&Idle,100) != OK)
  {
    return NO_MEMORY;
  };

  ChangeContext();
}




void PreInstallTasks(void)
{
  int i=0;
  DisableInterrupts;
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




/*
** ===================================================================
**     Interrupt handler : SwitchContext
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
//#pragma TRAP_PROC SAVE_NO_REGS
void interrupt SwitchContext(void)
{
  /* Write your interrupt code here ... */

  #if (HCS08 != 1)
  OS_SAVE_CONTEXT();
  #endif

  iNesting++;
  
  SaveCurrentSP();

  // Tratamento da interrupção
  
  iNesting--;
  
  if (iNesting == 0)
    OSSchedule();
  
  
  OS_RESTORE_CONTEXT();
  
  #if (Coldfire != 1)
    OSExitCritical;
  #endif
}
/* end of SwitchContext */

