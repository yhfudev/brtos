/**
* \file mbox.c
* \brief BRTOS MailBox functions
*
* Functions to install and use MailBoxes
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
*                                           OS MailBox functions
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
#include "OS_types.h"
#include "event.h"
#include "BRTOS.h"

#pragma warn_implicitconv off

#define ANY_WAITING    if (pont_event->OSEventWait != 0)
                                                                                            
#define REMOVE_FROM_WAITLIST                                                                  \
      iPriority = SAScheduler(pont_event->OSEventWaitList);                                   \
      pont_event->OSEventWaitList = pont_event->OSEventWaitList & ~(PriorityMask[iPriority]); \
      pont_event->OSEventWait--;                                                              

#define PUT_ON_READYLIST     OSReadyList = OSReadyList | (PriorityMask[iPriority]); 


#define MAILBOX_SET_MESSAGE                               \
    pont_event->OSEventPointer = message;                 \
    pont_event->OSEventState = AVAILABLE_MESSAGE;           
        
#define  FROM_TASK_SWITCH_CTX    if (!iNesting)  ChangeContext();
    
                                                                                    

#if (BRTOS_MBOX_EN == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create MailBox Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxCreate (BRTOS_Mbox **event, void *message)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT16S i = 0;  
  BRTOS_Mbox *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
      return(IRQ_PEND_ERR);                          // Can't be create by interrupt
  }
    
  // Enter critical Section
  if (currentTask)
     OSEnterCritical();
  
  // Verifica se ainda há blocos de controle de eventos disponíveis
  for(i=0;i<=BRTOS_MAX_MBOX;i++)
  {
    
    if(i == BRTOS_MAX_MBOX)
      // Caso não haja mais blocos disponíveis, retorna exceção
      return(NO_AVAILABLE_EVENT);
          
    
    if(BRTOS_Mbox_Table[i].OSEventAllocated != TRUE)
    {
      BRTOS_Mbox_Table[i].OSEventAllocated = TRUE;
      pont_event = &BRTOS_Mbox_Table[i];
      break;      
    }
  }    
    
  pont_event->OSEventPointer   = message;                // Link EventPtr to message
  pont_event->OSEventWait      = 0;  
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
/////      Delete Mutex Function                       /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSMboxDelete (BRTOS_Mbox **event)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  BRTOS_Mbox *pont_event;

  if (iNesting > 0) {                                // See if caller is an interrupt
      return(IRQ_PEND_ERR);                          // Can't be delete by interrupt
  }
    
  // Enter Critical Section
  OSEnterCritical();
  
  pont_event = *event;
  pont_event->OSEventAllocated   = 0;
  pont_event->OSEventPointer     = NULL;
  pont_event->OSEventWait        = 0;
  
  pont_event->OSEventWaitList=0;
  
  *event = NULL;
  
  // Exit Critical Section
  OSExitCritical();
  
  return(DELETE_EVENT_OK);
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

INT8U OSMboxPend (BRTOS_Mbox *pont_event, void *Mail, INT16U timeout)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U  iPriority = 0;
  ContextType *Task;
  
  // Can not use mailbox pend function from interrupt handling code
  if(iNesting > 0)
  {
    // Return NULL message
    Mail = (void *)NULL;
    // Indicates IRQ error
    return(IRQ_PEND_ERR);
  }
  
  // Enter Critical Section
  OSEnterCritical();
  
  // Verify if there was a message post
  if (pont_event->OSEventState == AVAILABLE_MESSAGE)
  {
    // Copy message pointer
    Mail = pont_event->OSEventPointer;
    
    // Free message slot
    pont_event->OSEventState = NO_MESSAGE;
    
    // Exit Critical Section
    OSExitCritical();
    return OK;
  }
  else
  {
    Task = &ContextTask[currentTask];
      
    // Copy task priority to local scope
    iPriority = Task->Priority;
    
    // Increases the semaphore wait list counter
    pont_event->OSEventWait++;
    
    // Allocates the current task on the mailbox wait list
    pont_event->OSEventWaitList = pont_event->OSEventWaitList | (PriorityMask[iPriority]);
    
    // Task entered suspended state, waiting for mailbox post
    #if (VERBOSE == 1)
    Task->State = SUSPENDED;
    Task->SuspendedType = MAILBOX;
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
    
    // Change Context - Returns on time overflow or mailbox post
    ChangeContext();

    #if (NESTING_INT == 1)
    // Exit Critical Section
    OSExitCritical();
    #endif
    
    // Enter Critical Section
    OSEnterCritical();

    // Verify if the reason of task wake up was mailbox timeout
    if(Task->TimeToWait == EXIT_BY_TIMEOUT)
    {      
      // Remove the task from the mailbox wait list
      pont_event->OSEventWaitList = pont_event->OSEventWaitList & ~(PriorityMask[iPriority]);
      
      // Decreases the mailbox wait list counter
      pont_event->OSEventWait--;
      
      // Return NULL message
      Mail = (void *)NULL;

      // Exit Critical Section
      OSExitCritical();
      
      // Indicates mailbox timeout
      return(TIMEOUT);
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
      
      // Copy message pointer
      Mail = pont_event->OSEventPointer;
      
      // Free message slot
      pont_event->OSEventState = NO_MESSAGE;
      
      // Exit Critical Section
      OSExitCritical();  
      return OK;      
    }
  }
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

INT8U OSMboxPost(BRTOS_Mbox *pont_event, void *message)
{
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U iPriority = (INT8U)0;
  INT8U TaskSelect = 0;  
  
  // Critical Section
  #if (NESTING_INT == 0)
  if (!iNesting)
  #endif
     OSEnterCritical();
       
  // See if any task is waiting for a message
  ANY_WAITING
  {
    // Selected task and remove from Waiting List
    REMOVE_FROM_WAITLIST
    
    // Put the selected task into Ready List
    PUT_ON_READYLIST     
    
    #if (VERBOSE == 1)
    TaskSelect = PriorityVector[iPriority]; 
    ContextTask[TaskSelect].State = READY;
    #endif
    
    MAILBOX_SET_MESSAGE
    
    FROM_TASK_SWITCH_CTX     
  
    #if (NESTING_INT == 0)                    
    if (!iNesting)                            
    #endif                                    
       OSExitCritical();  

    return OK;
  }
  else
  {

     MAILBOX_SET_MESSAGE 
     
      #if (NESTING_INT == 0)                    
      if (!iNesting)                            
      #endif    
      OSExitCritical();       
       
    
     return OK;
  }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
#endif


  
