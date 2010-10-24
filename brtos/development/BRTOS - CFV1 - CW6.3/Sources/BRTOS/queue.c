/**
* \file queue.c
* \brief BRTOS Queue functions
*
* Functions to install and use queues
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
*                                         OS Queue functions
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.1
*   Date:     11/03/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.2
*   Date:     01/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.3
*   Date:     11/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.4
*   Date:     19/10/2010
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.45
*   Date:     20/10/2010
*
*********************************************************************************************************/

#include "hardware.h"
#include "BRTOS.h"
#include "queue.h"
#include <stdlib.h>
#include "event.h"

#pragma warn_implicitconv off


#if (BRTOS_QUEUE_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Queue Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  int i=0;
  BRTOS_Queue *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be create by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
     OSEnterCritical();
  
  if ((iQueueAddress + size) > QUEUE_HEAP_SIZE)
  {
       return NO_MEMORY;
  }  
  
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
  cqueue->OSQPtr = (INT8U *)&QUEUE_STACK[iQueueAddress];
  iQueueAddress  = iQueueAddress + size;
  cqueue->OSQSize =  size;
  cqueue->OSQEntries =  0;
  cqueue->OSQStart = cqueue->OSQPtr;
  cqueue->OSQEnd =   cqueue->OSQPtr + cqueue->OSQSize;
  cqueue->OSQIn =    cqueue->OSQPtr;
  cqueue->OSQOut =   cqueue->OSQPtr;
  
  // Aloca tipo de evento e dados do evento
  pont_event->OSEventPointer = cqueue;
  pont_event->OSEventWait = 0;
  
  
  pont_event->OSEventWaitList=0;
  
  *event = pont_event;
  
  // Exit critical Section
  if (currentTask)
     OSExitCritical();  
  
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
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if (cqueue->OSQEntries < cqueue->OSQSize)
  {  
    cqueue->OSQEntries++;
  }
  else
  { 
     // Exit Critical Section
     #if (NESTING_INT == 0)
     if (!iNesting)
     #endif
       OSExitCritical();
       
     return BUFFER_UNDERRUN;
  }
  
  if (cqueue->OSQIn == cqueue->OSQEnd)
    cqueue->OSQIn = cqueue->OSQStart;
  
  *cqueue->OSQIn = data;
  cqueue->OSQIn++; 
 
   // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
      OSExitCritical();
  
  return WRITE_BUFFER_OK;
  
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

INT8U OSRQueue(OS_QUEUE *cqueue, INT8U* pdata)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif  
  
  // Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if(cqueue->OSQEntries > 0)
  {
      
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    *pdata = *(cqueue->OSQOut);
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return READ_BUFFER_OK;
  }
  else
  {
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return NO_ENTRY_AVAILABLE;
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

INT8U OSCleanQueue(BRTOS_Queue *pont_event)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  OS_QUEUE *cqueue = pont_event->OSEventPointer;
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  cqueue->OSQEntries = 0;
  
  cqueue->OSQIn = cqueue->OSQStart;
  cqueue->OSQOut = cqueue->OSQStart;
  
  // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
    OSExitCritical();  

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

INT8U OSQueuePend (BRTOS_Queue *pont_event, INT8U* pdata, INT16U timeout)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iPriority = 0;
  ContextType *Task;
  OS_QUEUE *cqueue = pont_event->OSEventPointer;
  
  // Can not use Queue pend function from interrupt handling code
  if(iNesting > 0)
  {
    return(IRQ_PEND_ERR);
  }

  // Enter Critical Section
  OSEnterCritical();
  
  // BRTOS TRACE SUPPORT
  #if (OSTRACE == 1) 
      #if(OS_TRACE_BY_TASK == 1)
      Update_OSTrace(currentTask, QUEUEPEND);
      #else
      Update_OSTrace(ContextTask[currentTask].Priority, QUEUEPEND);
      #endif
  #endif   
    
  // Verify if there is data in the queue
  if(cqueue->OSQEntries > 0)
  {
    // Verify for output pointer overflow
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    // Copy data from queue
    *pdata = *cqueue->OSQOut;
  
    // Increases output pointer
    cqueue->OSQOut++;
    
    // Decreases queue entries
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    OSExitCritical();
    return READ_BUFFER_OK;
  }
  else
  {
    Task = &ContextTask[currentTask];
    
    // Copy task priority to local scope
    iPriority = Task->Priority;
  
    // Increases the queue wait list counter
    pont_event->OSEventWait++;
    
    // Allocates the current task on the queue wait list
    pont_event->OSEventWaitList = pont_event->OSEventWaitList | (PriorityMask[iPriority]);
  
    // Task entered suspended state, waiting for queue post
    #if (VERBOSE == 1)
    Task->State = SUSPENDED;
    Task->SuspendedType = QUEUE;
    #endif

    // Remove current task from the Ready List
    OSReadyList = OSReadyList & ~(PriorityMask[iPriority]);
  
    // Set timeout overflow
    if (timeout)
    {  
      timeout = counter + timeout;
      if (timeout >= TickCountOverFlow)
        timeout = timeout - TickCountOverFlow;
      
    Task->TimeToWait = timeout;
    
    // Put task into delay list
    if(Tail != NULL)
    { 
      // Insert task into list
      Tail->Next = Task;
      Task->Previous = Tail;
      Tail = Task;
      Tail->Next = NULL;
    }
    else
    {
       // Init delay list
       Tail = Task;
       Head = Task; 
    }
    } else
    {
      Task->TimeToWait = NO_TIMEOUT;
    }
  
    // Change Context - Returns on time overflow or queue post
    ChangeContext();
    
    #if (NESTING_INT == 1)
    // Exit Critical Section
    OSExitCritical();
    #endif
    
    // Enter Critical Section
    OSEnterCritical();
    
    // Verify if the reason of task wake up was queue timeout
    if(Task->TimeToWait == EXIT_BY_TIMEOUT)
    {
        // Remove the task from the queue wait list
        pont_event->OSEventWaitList = pont_event->OSEventWaitList & ~(PriorityMask[iPriority]);
        // Decreases the queue wait list counter
        pont_event->OSEventWait--;
        
        // Exit Critical Section
        OSExitCritical();
        
        // Indicates queue timeout
        return TIMEOUT;
    }  
    // Remove the time to wait condition    
    else
    {
      if (timeout)
      {
          Task->TimeToWait = NO_TIMEOUT;
          
          // Remove from delay list
          if(Task == Head)
          {
            Head = Task->Next;
            Head->Previous = NULL;
            if(Task == Tail)
            {
              Tail = Task->Previous;
              Tail->Next = NULL;
            }          
          }
          else
          {          
            if(Task == Tail)
            {
              Tail = Task->Previous;
              Tail->Next = NULL;
            }
            else
            {
              Task->Next->Previous = Task->Previous;
              Task->Previous->Next = Task->Next; 
            }
          }    
      }
      
      // Verify for output pointer overflow
      if (cqueue->OSQOut == cqueue->OSQEnd)
        cqueue->OSQOut = cqueue->OSQStart;
    
      // Copy data from queue
      *pdata = *cqueue->OSQOut;
    
      // Increases the output pointer
      cqueue->OSQOut++;
      
      // Decreases queue entries
      cqueue->OSQEntries--;
      
      // Exit Critical Section
      OSExitCritical();
      return READ_BUFFER_OK;
    }
  }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Post Queue Function                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueuePost(BRTOS_Queue *pont_event, INT8U data)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif  
  INT8U iPriority = (INT8U)0;
  INT8U TaskSelect = 0;
  OS_QUEUE *cqueue = pont_event->OSEventPointer;
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
     
  // BRTOS TRACE SUPPORT
  #if (OSTRACE == 1)  
    if(!iNesting){ 
      #if(OS_TRACE_BY_TASK == 1)
      Update_OSTrace(currentTask, QUEUEPOST);
      #else
      Update_OSTrace(ContextTask[currentTask].Priority, QUEUEPOST);
      #endif
    }else{
      Update_OSTrace(0, QUEUEPOST);
    }
  #endif       
  
  // Checks for queue overflow
  if (cqueue->OSQEntries < cqueue->OSQSize)
  {  
    // If no, increases the queue entries
    cqueue->OSQEntries++;
  }
  else
  { 
     // Exit Critical Section
     #if (NESTING_INT == 0)
     if (!iNesting)
     #endif
       OSExitCritical();
     
     // Indicates queue overflow
     return BUFFER_UNDERRUN;
  }
  
  // Verify for input pointer overflow
  if (cqueue->OSQIn == cqueue->OSQEnd)
    cqueue->OSQIn = cqueue->OSQStart;
  
  // copy data into the queue
  *cqueue->OSQIn = data;
  
  // increases the input pointer
  cqueue->OSQIn++;
  
  // See if any task is waiting for new data in the queue
  if (pont_event->OSEventWait != 0)
  {
    // Selects the highest priority task
    iPriority = SAScheduler(pont_event->OSEventWaitList);
    TaskSelect = PriorityVector[iPriority];

    // Remove the selected task from the queue wait list
    pont_event->OSEventWaitList = pont_event->OSEventWaitList & ~(PriorityMask[iPriority]);
    
    // Decreases the queue wait list counter
    pont_event->OSEventWait--;
    
    // Put the selected task into Ready List
    #if (VERBOSE == 1)
    ContextTask[TaskSelect].State = READY;
    #endif
    
    OSReadyList = OSReadyList | (PriorityMask[iPriority]);
    
    // If outside of an interrupt service routine, change context to the highest priority task
    // If inside of an interrupt, the interrupt itself will change the context to the highest priority task
    if (!iNesting)
    {
      // Verify if there is a higher priority task ready to run
      ChangeContext();      
    }

    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
    
    return WRITE_BUFFER_OK;
  }
  else
  {
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
       OSExitCritical();
    
    return WRITE_BUFFER_OK;
  }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif



#if (BRTOS_QUEUE_16_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Queue Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueue16Create(OS_QUEUE_16 *cqueue, INT16U size, BRTOS_Queue **event)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT16S i=0;
  BRTOS_Queue *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be create by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
     OSEnterCritical();
  
  if ((iQueueAddress + (size*sizeof(INT16U))) > QUEUE_HEAP_SIZE)
  {
       return NO_MEMORY;
  }  
  
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
  cqueue->OSQPtr = (INT16U *)&QUEUE_STACK[iQueueAddress];
  iQueueAddress  = iQueueAddress + (size*sizeof(INT16U));
  cqueue->OSQSize =  size;
  cqueue->OSQEntries =  0;
  cqueue->OSQStart = cqueue->OSQPtr;
  cqueue->OSQEnd =   cqueue->OSQPtr + cqueue->OSQSize;
  cqueue->OSQIn =    cqueue->OSQPtr;
  cqueue->OSQOut =   cqueue->OSQPtr;
  
  // Aloca tipo de evento e dados do evento
  pont_event->OSEventPointer = cqueue;
  pont_event->OSEventWait = 0;    
  pont_event->OSEventWaitList=0;
  
  *event = pont_event;
  
  // Exit critical Section
  if (currentTask)
     OSExitCritical();  
  
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

INT8U OSWQueue16(OS_QUEUE_16 *cqueue,INT16U data)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if (cqueue->OSQEntries < cqueue->OSQSize)
  {  
    cqueue->OSQEntries++;
  }
  else
  { 
     // Exit Critical Section
     #if (NESTING_INT == 0)
     if (!iNesting)
     #endif
       OSExitCritical();
       
     return BUFFER_UNDERRUN;
  }
  
  if (cqueue->OSQIn == cqueue->OSQEnd)
    cqueue->OSQIn = cqueue->OSQStart;
  
  *cqueue->OSQIn = data;
  cqueue->OSQIn++;
  
   // Exit Critical Section
   #if (NESTING_INT == 0)
   if (!iNesting)
   #endif
      OSExitCritical();
  
  return WRITE_BUFFER_OK;
  
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

INT8U OSRQueue16(OS_QUEUE_16 *cqueue, INT16U *pdata)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
    
  // Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if(cqueue->OSQEntries > 0)
  {
      
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    *pdata = *cqueue->OSQOut;
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return READ_BUFFER_OK;
  }
  else
  {
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return NO_ENTRY_AVAILABLE;
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

INT8U OSCleanQueue16(OS_QUEUE_16 *cqueue)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif  
  OS_QUEUE_16 *tmp_queue = cqueue;
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  cqueue->OSQEntries = 0;
  
  cqueue->OSQIn = tmp_queue->OSQStart;
  cqueue->OSQOut = tmp_queue->OSQStart;
  
  // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
    OSExitCritical();  

  return CLEAN_BUFFER_OK;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif






#if (BRTOS_QUEUE_32_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Queue Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSQueue32Create(OS_QUEUE_32 *cqueue, INT16U size, BRTOS_Queue **event)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT16S i=0;
  BRTOS_Queue *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
     return(IRQ_PEND_ERR);                           // Can't be create by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
     OSEnterCritical();
  
  if ((iQueueAddress + (size*sizeof(INT32U))) > QUEUE_HEAP_SIZE)
  {
       return NO_MEMORY;
  }  
  
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
  cqueue->OSQPtr = (INT32U *)&QUEUE_STACK[iQueueAddress];
  iQueueAddress  = iQueueAddress + (size*sizeof(INT32U));
  cqueue->OSQSize =  size;
  cqueue->OSQEntries =  0;
  cqueue->OSQStart = cqueue->OSQPtr;
  cqueue->OSQEnd =   cqueue->OSQPtr + cqueue->OSQSize;
  cqueue->OSQIn =    cqueue->OSQPtr;
  cqueue->OSQOut =   cqueue->OSQPtr;
  
  // Aloca tipo de evento e dados do evento
  pont_event->OSEventPointer = cqueue;
  pont_event->OSEventWait = 0;    
  pont_event->OSEventWaitList=0;
  
  *event = pont_event;
  
  // Exit critical Section
  if (currentTask)
     OSExitCritical();  
  
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

INT8U OSWQueue32(OS_QUEUE_32 *cqueue,INT32U data)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if (cqueue->OSQEntries < cqueue->OSQSize)
  {  
    cqueue->OSQEntries++;
  }
  else
  { 
     // Exit Critical Section
     #if (NESTING_INT == 0)
     if (!iNesting)
     #endif
       OSExitCritical();
       
     return BUFFER_UNDERRUN;
  }
  
  if (cqueue->OSQIn == cqueue->OSQEnd)
    cqueue->OSQIn = cqueue->OSQStart;
  
  *cqueue->OSQIn = data;
  cqueue->OSQIn++;
  
   // Exit Critical Section
   #if (NESTING_INT == 0)
   if (!iNesting)
   #endif
      OSExitCritical();
  
  return WRITE_BUFFER_OK;
  
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

INT8U OSRQueue32(OS_QUEUE_32 *cqueue, INT32U *pdata)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
    
  // Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  if(cqueue->OSQEntries > 0)
  {
      
    if (cqueue->OSQOut == cqueue->OSQEnd)
      cqueue->OSQOut = cqueue->OSQStart;
  
    *pdata = *cqueue->OSQOut;
  
    cqueue->OSQOut++;
    cqueue->OSQEntries--;
    
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return READ_BUFFER_OK;
  }
  else
  {
    // Exit Critical Section
    #if (NESTING_INT == 0)
    if (!iNesting)
    #endif
      OSExitCritical();
      
    return NO_ENTRY_AVAILABLE;
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

INT8U OSCleanQueue32(OS_QUEUE_32 *cqueue)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif  
  OS_QUEUE_32 *tmp_queue = cqueue;
  
  // Enter Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
  
  cqueue->OSQEntries = 0;
  
  cqueue->OSQIn = tmp_queue->OSQStart;
  cqueue->OSQOut = tmp_queue->OSQStart;
  
  // Exit Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
    OSExitCritical();  

  return CLEAN_BUFFER_OK;
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif


