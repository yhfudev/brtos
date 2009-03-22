
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
*                                 OS Event Handle Structs Declarations
*
*
*   Author: Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/


#include "event.h"
#include "queue.h"

#include <stdlib.h>

INT16U iBRTOSEvent = 0;
INT16U iOS_Q = 0;





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Control Block Events Declaration            /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOSEvent_EN) && (BRTOS_MAX_EVENTS > 0)
  void             *OSEventFreeList;                    // Pointer to list of free EVENT control blocks
  BRTOSEvent       BRTOSEventTable[BRTOS_MAX_EVENTS];   // Table of EVENT control blocks
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Initialize Block List Control               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void initEvents(void)
{
  // Posiciona a lista de blocos de controle de eventos livres no inicio
  OSEventFreeList = &BRTOSEventTable[0];
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// Uma vari�vel global deve ser criada para controlar a entrada                   //
// em multiplas interrup��es. A troca de contexto s� deve ocorrer                 //
// quando est� vari�vel for igual a zero, ou seja, quando todas as                //
// interrup��es foram tratadas                                                    //
//                                                                                //
// Devem ser criadas fun��es para retirar e colocar dados no buffer circular      //
// Devem ser criadas fun��es                                                      //
//                                                                                //
// OBS:
// Uma interrup��o s� poder� gerar altera��o de contexto se a tarefa que acordou  //
// com o evento for de maior prioridade que a tarefa interrompida                 //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////
