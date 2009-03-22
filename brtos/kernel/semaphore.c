
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
#include <hidef.h> /* for EnableInterrupts macro */



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Semaphore Function                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemCreate (INT8U cnt, BRTOSEvent **event)
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
  pont_event->OSEventType    = BRTOSEvent_TYPE_SEM;
  pont_event->OSEventCount     = cnt;                      // Set semaphore count value
  pont_event->OSEventPointer     = (void *)0;                // Unlink from Event free list
  pont_event->OSEventWait = 0;
  
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
/////      Semaphore Pend Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemPend (BRTOSEvent *pont_event, INT16U timeout)
{
  INT16S i = 0;
  INT16U time_wait = 0;
  
  // Critical Section
  OSEnterCritical;  
  
  // Validate event block type
  if (pont_event->OSEventType != BRTOSEvent_TYPE_SEM)
  {
    OSExitCritical;
    return ERR_EVENT_TYPE;
  }
 
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
  
  ContextTask[currentTask].Suspended = TRUE;
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

INT8U OSSemPost(BRTOSEvent *pont_event)
{
  INT16U i=0;
  INT16U j=0;
  INT8U Max_Priority = 32;
  
  // Validate event block type
  if (pont_event->OSEventType != BRTOSEvent_TYPE_SEM)
  {   
    return ERR_EVENT_TYPE;;
  }
  
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
