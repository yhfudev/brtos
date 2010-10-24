/**
* \file queue.h
* \brief BRTOS Queue defines, function prototypes and struct declaration.
*
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
*                                      OS Queue functions Header
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#ifndef OS_QUEUE_H
#define OS_QUEUE_H

#include "OS_types.h"
#include "event.h"





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue Defines                               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// Enable Queue Structures
#define READ_BUFFER_OK       0            ///< New data successfully read
#define WRITE_BUFFER_OK      0            ///< New data successfully written
#define BUFFER_UNDERRUN      1            ///< Queue overflow
#define CLEAN_BUFFER_OK      2            ///< Queue successfully cleaned
#define NO_ENTRY_AVAILABLE   3            ///< Queue is empty

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue Structure                             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/**
* \struct OS_QUEUE
* Queue Control Block Structure
*/
typedef struct
{
  INT8U        *OSQPtr;                 ///< Pointer returned by the memory allocation function
  INT8U        *OSQStart;               ///< Pointer to the queue start
  INT8U        *OSQEnd;                 ///< Pointer to the queue end
  INT8U        *OSQIn;                  ///< Pointer to the next queue entry
  INT8U        *OSQOut;                 ///< Pointer to the next data in the queue output
  INT16U       OSQSize;                 ///< Size of the queue - Defined in the create queue function
  INT16U       OSQEntries;              ///< Size of data inside the queue
} OS_QUEUE;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue 16 Structure                          /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/**
* \struct OS_QUEUE
* Queue Control Block Structure
*/
typedef struct
{
  INT16U  *OSQPtr;                 ///< Pointer returned by the memory allocation function
  INT16U  *OSQStart;               ///< Pointer to the queue start
  INT16U  *OSQEnd;                 ///< Pointer to the queue end
  INT16U  *OSQIn;                  ///< Pointer to the next queue entry
  INT16U  *OSQOut;                 ///< Pointer to the next data in the queue output
  INT16U  OSQSize;                 ///< Size of the queue - Defined in the create queue function
  INT16U  OSQEntries;              ///< Size of data inside the queue
} OS_QUEUE_16;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue 32 Structure                          /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/**
* \struct OS_QUEUE
* Queue Control Block Structure
*/
typedef struct
{
  INT32U  *OSQPtr;                 ///< Pointer returned by the memory allocation function
  INT32U  *OSQStart;               ///< Pointer to the queue start
  INT32U  *OSQEnd;                 ///< Pointer to the queue end
  INT32U  *OSQIn;                  ///< Pointer to the next queue entry
  INT32U  *OSQOut;                 ///< Pointer to the next data in the queue output
  INT16U  OSQSize;                 ///< Size of the queue - Defined in the create queue function
  INT16U  OSQEntries;              ///< Size of data inside the queue
} OS_QUEUE_32;

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue Prototypes                            /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_QUEUE_EN == 1)

  /*****************************************************************************************//**
  * \fn INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event)
  * \brief Allocates a queue control block  
  * \param *cqueue Queue pointer
  * \param size Queue size
  * \param **event Queue event pointer
  * \return IRQ_PEND_ERR Can not use queue create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No queue control blocks available
  * \return ALLOC_EVENT_OK Queue control block successfully allocated
  *********************************************************************************************/
  INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event);
  
  /*****************************************************************************************//**
  * \fn OSWQueue(OS_QUEUE *cqueue,INT8U data)
  * \brief Writes new data in the specified queue
  * \param *cqueue Pointer to a queue
  * \param data Data to be written in the queue
  * \return BUFFER_UNDERRUN Queue overflow - There is no more available for new data
  * \return READ_BUFFER_OK New data successfully written
  *********************************************************************************************/
  INT8U OSWQueue(OS_QUEUE *cqueue,INT8U data);

  /*****************************************************************************************//**
  * \fn INT8U OSRQueue(OS_QUEUE *cqueue, INT8U* pdata)
  * \brief Reads new data from the specified queue
  * \param *cqueue Pointer to a queue
  * \param *pdata Pointer to data read - first data in the output buffer of the specified queue
  * \return READ_BUFFER_OK Data successfully read
  * \return NO_ENTRY_AVAILABLE There is no more available entry in queue
  *********************************************************************************************/
  INT8U OSRQueue(OS_QUEUE *cqueue, INT8U* pdata);
  
  /*****************************************************************************************//**
  * \fn INT8U OSCleanQueue(OS_QUEUE *cqueue)
  * \brief Clean data in the specified queue
  * \param *cqueue Pointer to a queue
  * \return CLEAN_BUFFER_OK Queue successfully cleaned
  *********************************************************************************************/  
  INT8U OSCleanQueue(BRTOS_Queue *pont_event);
  
  /*****************************************************************************************//**
  * \fn INT8U OSQueuePend (BRTOS_Queue *pont_event, OS_QUEUE *cqueue, INT16U timeout)
  * \brief Wait for a queue post 
  *  A task exits a pending state with a queue post or by timeout.
  * \param *pont_event Queue event pointer
  * \param *cqueue Queue pointer
  * \param timeout Timeout to the queue pend exits
  * \return First data in the output buffer of the specified queue
  *********************************************************************************************/
  INT8U OSQueuePend (BRTOS_Queue *pont_event, INT8U* pdata, INT16U timeout);
  
  /*****************************************************************************************//**
  * \fn INT8U OSQueuePost(BRTOS_Queue *pont_event, OS_QUEUE *cqueue,INT8U data)
  * \brief Queue post
  *  A task exits a pending state with a queue post or by timeout.
  * \param *pont_event Queue event pointer
  * \param *cqueue Queue pointer
  * \param data Data to be written in the queue
  * \param timeout Timeout to the queue pend exits
  * \return First data in the output buffer of the specified queue
  *********************************************************************************************/
  INT8U OSQueuePost(BRTOS_Queue *pont_event, INT8U data); 
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue 16 bits Prototypes                    /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_QUEUE_16_EN == 1)

  /*****************************************************************************************//**
  * \fn INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event)
  * \brief Allocates a queue control block  
  * \param *cqueue Queue pointer
  * \param size Queue size
  * \param **event Queue event pointer
  * \return IRQ_PEND_ERR Can not use queue create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No queue control blocks available
  * \return ALLOC_EVENT_OK Queue control block successfully allocated
  *********************************************************************************************/
  INT8U OSQueue16Create(OS_QUEUE_16 *cqueue, INT16U size, BRTOS_Queue **event);
  
  /*****************************************************************************************//**
  * \fn OSWQueue(OS_QUEUE *cqueue,INT8U data)
  * \brief Writes new data in the specified queue
  * \param *cqueue Pointer to a queue
  * \param data Data to be written in the queue
  * \return BUFFER_UNDERRUN Queue overflow - There is no more available for new data
  * \return READ_BUFFER_OK New data successfully written
  *********************************************************************************************/
  INT8U OSWQueue16(OS_QUEUE_16 *cqueue,INT16U data);

  /*****************************************************************************************//**
  * \fn INT8U OSRQueue(OS_QUEUE *cqueue)
  * \brief Reads new data from the specified queue
  * \param *cqueue Pointer to a queue
  * \param *pdata Pointer to data read - first data in the output buffer of the specified queue  
  * \return First data in the output buffer of the specified queue
  *********************************************************************************************/
  INT8U OSRQueue16(OS_QUEUE_16 *cqueue, INT16U *pdata);
  
  /*****************************************************************************************//**
  * \fn INT8U OSCleanQueue(OS_QUEUE *cqueue)
  * \brief Clean data in the specified queue
  * \param *cqueue Pointer to a queue
  * \return CLEAN_BUFFER_OK Queue successfully cleaned
  *********************************************************************************************/  
  INT8U OSCleanQueue16(OS_QUEUE_16 *cqueue);
    
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue 32 bits Prototypes                    /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_QUEUE_32_EN == 1)

  /*****************************************************************************************//**
  * \fn INT8U OSQueueCreate(OS_QUEUE *cqueue, INT16U size, BRTOS_Queue **event)
  * \brief Allocates a queue control block  
  * \param *cqueue Queue pointer
  * \param size Queue size
  * \param **event Queue event pointer
  * \return IRQ_PEND_ERR Can not use queue create function from interrupt handler code
  * \return NO_AVAILABLE_EVENT No queue control blocks available
  * \return ALLOC_EVENT_OK Queue control block successfully allocated
  *********************************************************************************************/
  INT8U OSQueue32Create(OS_QUEUE_32 *cqueue, INT16U size, BRTOS_Queue **event);
  
  /*****************************************************************************************//**
  * \fn OSWQueue(OS_QUEUE *cqueue,INT8U data)
  * \brief Writes new data in the specified queue
  * \param *cqueue Pointer to a queue
  * \param data Data to be written in the queue
  * \return BUFFER_UNDERRUN Queue overflow - There is no more available for new data
  * \return READ_BUFFER_OK New data successfully written
  *********************************************************************************************/
  INT8U OSWQueue32(OS_QUEUE_32 *cqueue,INT32U data);

  /*****************************************************************************************//**
  * \fn INT8U OSRQueue(OS_QUEUE *cqueue)
  * \brief Reads new data from the specified queue
  * \param *cqueue Pointer to a queue
  * \param *pdata Pointer to data read - first data in the output buffer of the specified queue  
  * \return First data in the output buffer of the specified queue
  *********************************************************************************************/
  INT8U OSRQueue32(OS_QUEUE_32 *cqueue, INT32U *pdata);
  
  /*****************************************************************************************//**
  * \fn INT8U OSCleanQueue(OS_QUEUE *cqueue)
  * \brief Clean data in the specified queue
  * \param *cqueue Pointer to a queue
  * \return CLEAN_BUFFER_OK Queue successfully cleaned
  *********************************************************************************************/  
  INT8U OSCleanQueue32(OS_QUEUE_32 *cqueue);
    
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#endif
