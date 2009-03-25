
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
*                                       OS Semaphore functions
*
*
*   Author: Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "OS_types.h"
#include "event.h"
#include "BRTOS.h"
#include "hardware.h"


#if (BRTOS_SEM_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Semaphore Function                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemCreate (INT8U cnt, BRTOS_Sem **event)
{
  int i=0;

  BRTOS_Sem *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be create by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
     OSEnterCritical;
  
  // Verifica se ainda há blocos de controle de eventos disponíveis
  for(i=0;i<=BRTOS_MAX_SEM;i++)
  {
    
    if(i == BRTOS_MAX_SEM)
      // Caso não haja mais blocos disponíveis, retorna exceção
      return(NO_AVAILABLE_EVENT);
          
    
    if(BRTOS_Sem_Table[i].OSEventAllocated != TRUE)
    {
      BRTOS_Sem_Table[i].OSEventAllocated = TRUE;
      pont_event = &BRTOS_Sem_Table[i];
      break;      
    }
  }
  
    // Exit Critical
  pont_event->OSEventCount = cnt;                      // Set semaphore count value
  pont_event->OSEventWait  = 0;
  
  for(i=0;i<WAIT_LIST_SIZE;i++)
  {
    pont_event->OSEventList[i]=0;  
  }
  
  *event = pont_event;
  
  // Exit critical Section
  if (currentTask)
     OSExitCritical;
  
  return(ALLOC_EVENT_OK);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Delete Semaphore Function                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemDelete (BRTOS_Sem **event)
{
  INT16S i = 0;
  BRTOS_Sem *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
      return(IRQ_PEND_ERR);                          // Can't be delete by interrupt
  }
    
  // Enter Critical Section
  OSEnterCritical;
  
  pont_event = *event;  
  pont_event->OSEventAllocated = 0;
  pont_event->OSEventCount     = 0;                      
  pont_event->OSEventWait      = 0;
  
  for(i=0;i<WAIT_LIST_SIZE;i++)
  {
    pont_event->OSEventList[i]=0;  
  }
  
  *event = NULL;
  
  // Exit Critical Section
  OSExitCritical;
  
  return(DELETE_EVENT_OK);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Semaphore Pend Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemPend (BRTOS_Sem *pont_event, INT16U timeout)
{
  INT16S i = 0;
  INT16U time_wait = 0;
  
  // Critical Section
  OSEnterCritical;  
 
  if (pont_event->OSEventCount > 0)
  {
    pont_event->OSEventCount--;
    
    // Exit Critical Section
    OSExitCritical;
    return OK;
  }
 
  
  if(iNesting > 0)
  {
    return(IRQ_PEND_ERR);
  }
  
  ContextTask[currentTask].State = SUSPENDED;
  ContextTask[currentTask].SuspendedType = SEMAPHORE;

  
  // Verificar o timeout em relação ou counter overflow
  if (timeout)
  {  
    time_wait = counter + timeout;
    if (time_wait >= TickCountOverFlow)
      time_wait = timeout - (TickCountOverFlow - counter);
    ContextTask[currentTask].TimeToWait = time_wait;
  } else
    ContextTask[currentTask].TimeToWait = 65000;
  
  
  
  // Aloca a tarefa com semaforo pendente na lista do evento
  for(i=0;i<WAIT_LIST_SIZE;i++)
  {
    if(pont_event->OSEventList[i] == currentTask)
    {
      break;
    }
      
    if(pont_event->OSEventList[i] == 0)
    {
      pont_event->OSEventList[i] = currentTask;
      pont_event->OSEventWait++;
      break;
    }
  }
  
  // Troca contexto e só retorna quando estourar Delay ou quando ocorrer um Post do Semaforo
  ChangeContext();
  return OK;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Semaphore Post Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemPost(BRTOS_Sem *pont_event)
{
  INT16U i=0;
  INT16U j=0;
  INT8U Max_Priority = 32;
  
  // Enter Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;
  
  if (pont_event->OSEventWait != 0)                    // See if any task is waiting for semaphore
  {
    for(i=0;i<WAIT_LIST_SIZE;i++)
    {
	    if (pont_event->OSEventList[i] != 0)
	    {
	      if(ContextTask[pont_event->OSEventList[i]].Priority <= Max_Priority)
	      {
	        // Descobre a tarefa de maior prioridade esperando o recurso
	        Max_Priority = ContextTask[pont_event->OSEventList[i]].Priority;
	        j = i;
	      }
      }
    }
    // Coloca a tarefa de maior prioridade na Ready List
    ContextTask[pont_event->OSEventList[j]].State = READY;     // Coloca a tarefa na Ready List
    // Retira esta tarefa das tarefas que esperam por evento / recurso
    pont_event->OSEventList[j] = 0;
    // Decrementa o número de tarefas esperando pelo evento / recurso
    pont_event->OSEventWait--;
    
    // Exit Critical Section
    #if (Coldfire == 1)
    if (!iNesting)
    #endif
      OSExitCritical;
    return OK;
  }
    
  if (pont_event->OSEventCount < 255)             // Make sure semaphore will not overflow
  {            
    pont_event->OSEventCount++;                   // Increment semaphore count
                         
    // Exit Critical Section
    #if (Coldfire == 1)
    if (!iNesting)
    #endif
       OSExitCritical;
    return OK;
  }
  // Exit Critical Section             
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSExitCritical;
  return ERR_SEM_OVF;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#endif
