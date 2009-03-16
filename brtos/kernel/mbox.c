#include "types.h"
#include "event.h"
#include "myRTOS.h"



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create MailBox Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxCreate (void *message, OS_EVENT **event)
{
  OS_EVENT *pont_event;
  INT16S i = 0;

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
  pont_event->OSEventType    = OS_EVENT_TYPE_MBOX;
  pont_event->OSEventCnt     = 0;
  pont_event->OSEventPtr     = message;                // Link EventPtr to message
  pont_event->OSEventGrp = 0;
  
  for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
  {
    pont_event->OSEventTbl[i]=0;  
  }
  
  *event = pont_event;
  
  return(ALLOC_EVENT_OK);
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mailbox Pend Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void *OSMboxPend (OS_EVENT *pont_event, INT16U timeout)
{
  INT16U time_wait = 0;
  INT16S i = 0;  
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_MBOX)
  {
    return (void *)0;
  }
  
  if(iNesting > 0)
  {
    return (void *)0;
  }
  
  asm(MOVE.w #0x2700,SR);
  
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
  return (pont_event->OSEventPtr);
}



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mailbox Post Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxPost(OS_EVENT *pont_event, void *message)
{
  int i=0;
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_MBOX)
  {   /* Validate event block type                     */
    return ERR_EVENT_TYPE;;
  }
  
  // Enter Critical
  if (pont_event->OSEventGrp != 0)                    /* See if any task waiting for Message     */
  {
    for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
    {
	    if (pont_event->OSEventTbl[i] != 0)
	    {    
        ContextTask[pont_event->OSEventTbl[i]].Suspended = READY;     // Coloca a tarefa na Ready List
      }
    }
    // Exit Critical
    pont_event->OSEventPtr = message;
    asm(MOVE.w #0x2700,SR);
    ChangeContext();
    return OK;
  }
  
  return ERR_EVENT_NO_CREATED;
    
}
