
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
*                                          OS Mutex functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "OS_types.h"
#include "event.h"
#include "BRTOS.h"
#include <hidef.h> /* for EnableInterrupts macro */





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Mutex Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexCreate (BRTOSEvent **event, INT8U HigherPriority)
{
  int i=0;

  BRTOSEvent *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
      return(IRQ_PEND_ERR);                          // Can't be create by interrupt
  }
    
  // Enter critical
  // Verifica se ainda há blocos de controle de eventos disponíveis
  if (iBRTOSEvent < BRTOS_MAX_EVENTS)
  {
      iBRTOSEvent++;
      pont_event = OSEventFreeList;
      
      // Posiciona a lista de blocos de controle de eventos livre no próximo bloco disponível
      OSEventFreeList = &BRTOSEventTable[iBRTOSEvent];
  } else
  {
    // Caso não haja mais blocos disponíveis, retorna exceção
    return(NO_AVAILABLE_EVENT);
  }    
    

    // Exit Critical
  pont_event->OSEventType    = BRTOSEvent_TYPE_MUTEX;
  pont_event->OSEventCount     = 1;                        // Set mutex counter value
  pont_event->OSEventPointer     = (void *)0;                // Unlink from Event free list
  pont_event->OSEventWait = 0;
  // Determina a tarefa de maior prioridade acessando o mutex
  pont_event->OSMaxPriority = HigherPriority;

  
  for(i=0;i<WAIT_LIST_SIZE;i++)
  {
    pont_event->OSEventList[i]=0;  
  }
  
  *event = pont_event;
  
  return(ALLOC_EVENT_OK);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mutex Pend Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexPend (BRTOSEvent *pont_event, INT16U timeout)
{
  INT16S i = 0;
  INT16U time_wait = 0;
  INT8U backup_task;

  // Enter critical section
  OSEnterCritical;

  // Validate event block type  
  if (pont_event->OSEventType != BRTOSEvent_TYPE_MUTEX)
  {
    OSExitCritical;
    return ERR_EVENT_TYPE;
  }
  
  if (pont_event->OSEventCount > 0)
  {
    pont_event->OSEventCount--;
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
  
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mutex Post Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMutexPost(BRTOSEvent *pont_event)
{
  INT16U i=0;
  INT16U j=0;
  INT8U Max_Priority = 32;
  INT8U backup_task = 0;

  // Validate event block type  
  if (pont_event->OSEventType != BRTOSEvent_TYPE_MUTEX)
  {
    return ERR_EVENT_TYPE;
  }

  // Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;  

  // Verify Mutex Owner
  if (pont_event->OSEventOwner != currentTask)
  {   
    return ERR_EVENT_OWNER;
  }  
  
  if (pont_event->OSEventWait != 0)                    // See if any task is waiting for mutex
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
    
    pont_event->OSOriginalPriority = ContextTask[pont_event->OSEventList[j]].Priority;
    
    if (pont_event->OSMaxPriority != ContextTask[pont_event->OSEventList[j]].Priority)
    {
       // Event Context Priority Information
       ContextTask[pont_event->OSEventList[j]].Priority = pont_event->OSMaxPriority;
       
       // Priority change
       backup_task = PriorityVector[pont_event->OSMaxPriority];
       PriorityVector[pont_event->OSMaxPriority] = pont_event->OSEventList[j];
       PriorityVector[pont_event->OSOriginalPriority] = backup_task;
    }    
    
    // Coloca a tarefa de maior prioridade na Ready List
    ContextTask[pont_event->OSEventList[j]].Suspended = READY;     // Coloca a tarefa na Ready List
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
    
  if (pont_event->OSEventCount < 255)               // Make sure mutex will not overflow
  {            
    pont_event->OSEventCount++;                     // Increment mutex count

    
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
  
  return ERR_MUTEX_OVF;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
