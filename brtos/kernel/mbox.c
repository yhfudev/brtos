
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
*                                           OS MailBox functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include <hidef.h> /* for EnableInterrupts macro */
#include "OS_types.h"
#include "event.h"
#include "BRTOS.h"



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create MailBox Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxCreate (void *message, BRTOSEvent **event)
{
  BRTOSEvent *pont_event;
  INT16S i = 0;

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
  pont_event->OSEventType    = BRTOSEvent_TYPE_MBOX;
  pont_event->OSEventCount     = 0;
  pont_event->OSEventPointer     = message;                // Link EventPtr to message
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
/////      Mailbox Pend Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void *OSMboxPend (BRTOSEvent *pont_event, INT16U timeout)
{
  INT16U time_wait = 0;
  INT16S i = 0;  
  
  // Enter Critical Section
  OSEnterCritical;  

  // Validate event block type  
  if (pont_event->OSEventType != BRTOSEvent_TYPE_MBOX)
  {
    OSExitCritical;
    return (void *)0;
  }
  
  if(iNesting > 0)
  {
    OSExitCritical;
    return (void *)0;
  }
  
  ContextTask[currentTask].Suspended = TRUE;
  ContextTask[currentTask].SuspendedType = MAILBOX;
  
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
  return (pont_event->OSEventPointer);
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mailbox Post Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxPost(BRTOSEvent *pont_event, void *message)
{
  int i=0;

  // Validate event block type  
  if (pont_event->OSEventType != BRTOSEvent_TYPE_MBOX)
  {
    return ERR_EVENT_TYPE;;
  }
  
  // Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;
       
  if (pont_event->OSEventWait != 0)                                  // See if any task is waiting for Message
  {
    for(i=0;i<WAIT_LIST_SIZE;i++)
    {
	    if (pont_event->OSEventList[i] != 0)
	    {    
        ContextTask[pont_event->OSEventList[i]].Suspended = READY;   // Coloca a tarefa na Ready List
      }
    }
    pont_event->OSEventPointer = message;
    
    
    ChangeContext();
    return OK;
  }
  
  return ERR_EVENT_NO_CREATED;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
