/**
* \file HAL.c
* \brief BRTOS Hardware Abstraction Layer Functions.
*
* This file contain the functions that are processor dependant.
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
*                                   OS HAL Functions to Coldfire V1
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "BRTOS.h"

#if (SP_SIZE == 16)
  INT16U SPvalue;                             ///< Used to save and restore a task stack pointer
#endif


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Tick Timer Setup                         /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void TickTimerSetup(void)
{
  
    //BCSCTL3 |= LFXT1S_2;  // VLO
	
	/* Ensure the timer is stopped. */
	TACTL = 0;

	/* Run the timer of the ACLK. */
	TACTL = TASSEL_2;

	/* Clear everything to start with. */
	TACTL |= TACLR;

	/* Set the compare match value according to the tick rate we want. */
	TACCR0 = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) >> configTIMER_PRE_SCALER; /* Period value setting */   

	/* Enable the interrupts. */
	TACCTL0 = CCIE;

	/* Start up clean. */
	TACTL |= TACLR;

	/* Up mode. */
	TACTL |= MC_1;
  
  /* ### */
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS RTC Setup                                /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void OSRTCSetup(void)
{  
	// not used
	__asm ("nop");

  //OSResetTime(&Hora);
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void TickTimer (void)
{
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();
  
    // Interrupt handling
  TICKTIMER_INT_HANDLER;

  counter++;
  if (counter == TickCountOverFlow) counter = 0;
  
  // BRTOS TRACE SUPPORT
  #if (OSTRACE == 1) 
      #if(OS_TICK_SHOW == 1) 
          #if(OS_TRACE_BY_TASK == 1)
          Update_OSTrace(0, ISR_TICK);
          #else
          Update_OSTrace(configMAX_TASK_INSTALL - 1, ISR_TICK);
          #endif         
      #endif       
  #endif  

  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif   
    
       
  // ************************
  // Handler code for the tick
  // ************************
  OS_TICK_HANDLER();
  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT();  
  // ************************  
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////   Software Interrupt to provide Switch Context   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
 /************************************************************//**
* \fn fake interrupt void SwitchContext(void)
* \brief Software interrupt handler routine (Internal kernel function).
*  Used to switch the tasks context.
****************************************************************/

void SwitchContext(void)
{
  
  // as MSP430 does not have sw interrupt, we save 7 regs to make it appear like one.
  // save 7 regs
  OS_SAVE_ISR();
  
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();
  
  // Interrupt Handling  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT(); 

  // ************************
  OS_RESTORE_ISR();
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////  Task Installation Function                      /////
/////                                                  /////
/////  Parameters:                                     /////
/////  Function pointer, task priority and task name   /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES)
{  
   // First SR should be 0
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 4] = 0x08;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 3] = 0x00;
	  
   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned int) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned int) (FctPtr)) >> 8;   

	// Initialize registers
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x15;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x14;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 9] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 10] = 0x13;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 11] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 12] = 0x12;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 14] = 0x11;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 15] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 16] = 0x10;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 18] = 0x09;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 19] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 20] = 0x08;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 22] = 0x07;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 23] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 24] = 0x06;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 26] = 0x05;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 27] = 0x00;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 28] = 0x04;
   
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
