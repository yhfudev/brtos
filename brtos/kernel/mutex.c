#include "types.h"
#include "event.h"
#include "BRTOS.h"
#include <hidef.h> /* for EnableInterrupts macro */



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Mutex Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexCreate (OS_EVENT **event, INT8U HigherPriority)
{
  int i=0;

  OS_EVENT *pont_event;

  if (iNesting > 0) {                                /* See if called from ISR ...               */
      return(IRQ_PEND_ERR);                          /* ... can't CREATE from an ISR             */
  }
    
  // Enter critical
  // Verifica se ainda há blocos de controle de eventos disponíveis
  if (iOS_Event < OS_MAX_EVENTS)
  {
      iOS_Event++;
      pont_event = OSEventFreeList;
      
      // Posiciona a lista de blocos de controle de eventos livre no próximo bloco disponível
      OSEventFreeList = &OSEventTbl[iOS_Event];
  } else
  {
    // Caso não haja mais blocos disponíveis, retorna exceção
    return(NO_AVAILABLE_EVENT);
  }    
    

    // Exit Critical
  pont_event->OSEventType    = OS_EVENT_TYPE_MUTEX;
  pont_event->OSEventCnt     = 1;                        /* Set semaphore value                      */
  pont_event->OSEventPtr     = (void *)0;                /* Unlink from ECB free list                */
  pont_event->OSEventGrp = 0;
  // Determina a tarefa de maior prioridade acessando o mutex
  pont_event->OSMaxPriority = HigherPriority;

  
  for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
  {
    pont_event->OSEventTbl[i]=0;  
  }
  
  *event = pont_event;
  
  return(ALLOC_EVENT_OK);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mutex Pend Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexPend (OS_EVENT *pont_event, INT16U timeout)
{
  INT16S i = 0;
  INT16U time_wait = 0;
  INT8U backup_task;
  
  OSEnterCritical;
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_MUTEX)
  {
    OSExitCritical;
    return ERR_EVENT_TYPE;
  }
 
  
  if (pont_event->OSEventCnt > 0)
  {
    pont_event->OSEventCnt--;
    pont_event->OSEventOwner = currentTask;
        
    ///////////////////////////////////////////////////////////////////////////////
    // Realiza a troca temporária de prioridade da tarefa proprietária do mutex  //
    // No caso da prioridade da tarefa já não ser a maior                        //
    // OBS: menor valor de prioridade indica maior prioridade                    //
    ///////////////////////////////////////////////////////////////////////////////
    
    pont_event->OSOriginalPriority = ContextTask[currentTask].Priority;
    
    if (pont_event->OSMaxPriority != ContextTask[currentTask].Priority)
    {
       // Event Context Priority Information
       ContextTask[currentTask].Priority = pont_event->OSMaxPriority;
       
       // Priority change
       backup_task = PriorityVector[pont_event->OSMaxPriority];
       PriorityVector[pont_event->OSMaxPriority] = currentTask;
       PriorityVector[pont_event->OSOriginalPriority] = backup_task;
    }
    
    OSExitCritical;
    return OK;
  }
 
  
  if(iNesting > 0)
  {
    return(IRQ_PEND_ERR);
  }
  
  ContextTask[currentTask].Suspended = TRUE;
  ContextTask[currentTask].SuspendedType = MUTEX;

  
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
  for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
  {
    if(pont_event->OSEventTbl[i] == currentTask)
    {
      break;
    }
      
    if(pont_event->OSEventTbl[i] == 0)
    {
      pont_event->OSEventTbl[i] = currentTask;
      pont_event->OSEventGrp++;
      break;
    }
  }
  
  // Troca contexto e só retorna quando estourar Delay ou quando ocorrer um Post do Semaforo
  ChangeContext();
  
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mutex Post Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexPost(OS_EVENT *pont_event)
{
  INT16U i=0;
  INT16U j=0;
  INT8U Max_Priority = 0;
  INT8U backup_task = 0;
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_MUTEX)
  {   /* Validate event block type                     */
    return ERR_EVENT_TYPE;
  }
  
  
  if (pont_event->OSEventOwner != currentTask)
  {   /* Validate event block type                     */
    return ERR_EVENT_OWNER;
  }  
  
  
  if (iNesting == 0)
    OSEnterCritical;
  
  
  
  if (pont_event->OSEventGrp != 0)                    /* See if any task waiting for semaphore         */
  {
    for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
    {
	    if (pont_event->OSEventTbl[i] != 0)
	    {
	      if(ContextTask[pont_event->OSEventTbl[i]].Priority >= Max_Priority)
	      {
	        // Descobre a tarefa de maior prioridade esperando o recurso
	        Max_Priority = ContextTask[pont_event->OSEventTbl[i]].Priority;
	        j = i;
	      }
      }
    }
    
    // Realiza a volta da troca temporária de prioridade da tarefa proprietária do mutex
    // OBS: menor valor de prioridade indica maior prioridade
    if (pont_event->OSOriginalPriority != ContextTask[currentTask].Priority)
    {
       // Event Context Priority Information
       ContextTask[currentTask].Priority = pont_event->OSOriginalPriority;
       
       // Priority change
       backup_task = PriorityVector[pont_event->OSOriginalPriority];
       PriorityVector[pont_event->OSOriginalPriority] = currentTask;
       PriorityVector[pont_event->OSMaxPriority] = backup_task;
    }
    
    ///////////////////////////////////////////////////////////////////////////////
    // Realiza a troca temporária de prioridade da tarefa proprietária do mutex  //
    // No caso da prioridade da tarefa já não ser a maior                        //
    // OBS: menor valor de prioridade indica maior prioridade                    //
    ///////////////////////////////////////////////////////////////////////////////
    
    pont_event->OSOriginalPriority = ContextTask[pont_event->OSEventTbl[j]].Priority;
    
    if (pont_event->OSMaxPriority != ContextTask[pont_event->OSEventTbl[j]].Priority)
    {
       // Event Context Priority Information
       ContextTask[pont_event->OSEventTbl[j]].Priority = pont_event->OSMaxPriority;
       
       // Priority change
       backup_task = PriorityVector[pont_event->OSMaxPriority];
       PriorityVector[pont_event->OSMaxPriority] = pont_event->OSEventTbl[j];
       PriorityVector[pont_event->OSOriginalPriority] = backup_task;
    }    
    
    // Coloca a tarefa de maior prioridade na Ready List
    ContextTask[pont_event->OSEventTbl[j]].Suspended = READY;     // Coloca a tarefa na Ready List
    // Retira esta tarefa das tarefas que esperam por evento / recurso
    pont_event->OSEventTbl[j] = 0;
    // Decrementa o número de tarefas esperando pelo evento / recurso
    pont_event->OSEventGrp--;
    
    
    
    // Exit Critical
    if (iNesting == 0)
      OSExitCritical;
    return OK;
  }
    
  if (pont_event->OSEventCnt < 65535u)          /* Make sure semaphore will not overflow         */
  {            
    pont_event->OSEventCnt++;                     /* Increment semaphore count to register event   */

    
    // Realiza a volta da troca temporária de prioridade da tarefa proprietária do mutex
    // OBS: menor valor de prioridade indica maior prioridade
    if (pont_event->OSOriginalPriority != ContextTask[currentTask].Priority)
    {
       // Event Context Priority Information
       ContextTask[currentTask].Priority = pont_event->OSOriginalPriority;
       
       // Priority change
       backup_task = PriorityVector[pont_event->OSOriginalPriority];
       PriorityVector[pont_event->OSOriginalPriority] = currentTask;
       PriorityVector[pont_event->OSMaxPriority] = backup_task;
    }
    
        
    // Exit Critical
    if (iNesting == 0)
      OSExitCritical;
    return OK;
  }
  // Exit Critical                           /* Semaphore value has reached its maximum       */
  if (iNesting == 0)
    OSExitCritical;
  return ERR_MUTEX_OVF;
}
