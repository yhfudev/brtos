
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
*                                         OS Queue functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include <hidef.h> /* for EnableInterrupts macro */
#include "BRTOS.h"
#include "queue.h"
#include <stdlib.h>
#include "event.h"




#if (BRTOS_QUEUE_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Queue Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event)
{
  int i=0;
  BRTOS_Queue *pont_event;

  
  // Enter critical
  // Verifica se ainda há blocos de controle de eventos disponíveis
  for(i=0;i<=BRTOS_MAX_QUEUE;i++)
  {
    
    if(i == BRTOS_MAX_QUEUE)
      // Caso não haja mais blocos disponíveis, retorna exceção
      return(NO_AVAILABLE_EVENT);
          
    
    if(BRTOS_Queue_Table[i].OSEventAllocated != TRUE)
    {
      BRTOS_Queue_Table[i].OSEventAllocated = TRUE;
      pont_event = &BRTOS_Queue_Table[i];
      break;      
    }
  } 
  
  // Configura dados de evento de lista
  cqueue->OSQPtr = (INT8U *) calloc(size,sizeof(INT8U));
  cqueue->OSQSize =  size;
  cqueue->OSQEntries =  0;
  cqueue->OSQStart = cqueue->OSQPtr;
  cqueue->OSQEnd =   cqueue->OSQPtr + cqueue->OSQSize;
  cqueue->OSQIn =    cqueue->OSQPtr;
  cqueue->OSQOut =   cqueue->OSQPtr;
  
  // Aloca tipo de evento e dados do evento
  pont_event->OSEventPointer = cqueue;
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
/////      Write Queue Function                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSWQueue(OS_QUEUE *cqueue,INT8U data)
{
  // Enter Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;
  
  if (cqueue->OSQEntries < cqueue->OSQSize)
  {  
    cqueue->OSQEntries++;
  }
  else
  { 
     // Exit Critical Section
     #if (Coldfire == 1)
     if (!iNesting)
     #endif
       OSExitCritical;
     return BUFFER_UNDERRUN;
  }
  
  if (cqueue->OSQIn == cqueue->OSQEnd)
    cqueue->OSQIn = cqueue->OSQStart;
  
  *cqueue->OSQIn = data;
  cqueue->OSQIn++;
  
   // Exit Critical Section
   #if (Coldfire == 1)
   if (!iNesting)
   #endif
      OSExitCritical;
  
  return READ_BUFFER_OK;
  
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Read Queue Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSRQueue(OS_QUEUE *cqueue)
{
  INT8U data;
  
  // Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;
  
  if(cqueue->OSQEntries > 0)
  {
      
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    data = *cqueue->OSQOut;
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    // Exit Critical Section
    #if (Coldfire == 1)
    if (!iNesting)
    #endif
      OSExitCritical;
    return data;
  }
  else
  {
    // Exit Critical Section
    #if (Coldfire == 1)
    if (!iNesting)
    #endif
      OSExitCritical;
    return (INT8U)NULL;
  }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Clean Queue Function                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSCleanQueue(OS_QUEUE *cqueue)
{
  // Enter Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
     OSEnterCritical;
  
  cqueue->OSQEntries = 0;
  cqueue->OSQIn = cqueue->OSQStart;
  cqueue->OSQOut = cqueue->OSQStart;
  
  // Exit Critical Section
  #if (Coldfire == 1)
  if (!iNesting)
  #endif
    OSExitCritical;  

  return CLEAN_BUFFER_OK;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue Pend Function                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueuePend (BRTOS_Queue *pont_event, OS_QUEUE *cqueue, INT16U timeout)
{
  INT16S i = 0;
  INT8U data;  
  INT16U time_wait = 0;
  
  // Critical Section
  OSEnterCritical;  
    
  if(cqueue->OSQEntries > 0)
  {
      
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    data = *cqueue->OSQOut;
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    OSExitCritical;
    return data;
  }
  else
  {
    ContextTask[currentTask].Suspended = TRUE;
    ContextTask[currentTask].SuspendedType = QUEUE;

  
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
    
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    data = *cqueue->OSQOut;
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    OSExitCritical;
    return data;

  }
  
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#endif
