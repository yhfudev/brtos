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
*                                     OS HAL Header for ARM Cortex-M4
*
*
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     30/04/2011
*
*********************************************************************************************************/

#include "BRTOS.h"
#include <cr_section_macros.h>
#include <NXP/crp.h>

void TickTimer(void) __attribute__ ((naked));
void SwitchContext(void) __attribute__ (( naked ));
void SwitchContextToFirstTask(void) __attribute__ (( naked ));
INT32U OS_CPU_SR_Save(void) __attribute__ (( naked ));
void OS_CPU_SR_Restore(INT32U prio) __attribute__ (( naked ));




#if (SP_SIZE == 32)
  INT32U SPvalue;                             ///< Used to save and restore a task stack pointer
#endif

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
	if (SysTick_Config(configCPU_CLOCK_HZ / (INT32U)configTICK_RATE_HZ)) { /* Setup SysTick Timer for 1 msec interrupts  */
		while (1);                                  /* Capture error */
	}
	
	if ( !(SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) )
	{
		/* When external reference clock is used(CLKSOURCE in
		Systick Control and register bit 2 is set to 0), the
		SYSTICKCLKDIV must be a non-zero value and 2.5 times
		faster than the reference clock.
		When core clock, or system AHB clock, is used(CLKSOURCE
		in Systick Control and register bit 2 is set to 1), the
		SYSTICKCLKDIV has no effect to the SYSTICK frequency. See
		more on Systick clock and status register in Cortex-M3
		technical Reference Manual. */
		LPC_SYSCON->SYSTICKCLKDIV = 0x08;
	}
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
 
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void TickTimer(void)
{
  // ************************
  // Entrada de interrup��o
  // ************************
  OS_SAVE_ISR();
  OS_INT_ENTER();
  
  // Interrupt handling
  TICKTIMER_INT_HANDLER;

  OSIncCounter();
  
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
  OS_RESTORE_ISR();  
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
* \fn interrupt void SwitchContext(void)
* \brief Software interrupt handler routine (Internal kernel function).
*  Used to switch the tasks context.
****************************************************************/
void SwitchContext(void)
{
  // ************************
  // Entrada de interrupcao
  // ************************
  OS_SAVE_ISR();
  OS_INT_ENTER();

  // Interrupt Handling
  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT();  
  OS_RESTORE_ISR();
  // ************************
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


void SwitchContextToFirstTask(void)
{
	OS_RESTORE_SP();
	OS_RESTORE_CONTEXT();
	OS_RESTORE_ISR();
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////  Task Installation Function                      /////
/////                                                  /////
/////  Parameters:                                     /////
/////  Function pointer, task priority and task name   /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void          OS_TaskReturn             (void);


void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES)
{  
	INT32U *stk_pt = (INT32U*)&STACK[iStackAddress + (NUMBER_OF_STACKED_BYTES / sizeof(OS_CPU_TYPE))];
	
	*--stk_pt = (INT32U)INITIAL_XPSR;                   	/* xPSR                                                   */
    *--stk_pt = (INT32U)FctPtr;                             /* Entry Point                                            */
    /// ??????????????????????
    *--stk_pt = (INT32U)0;                      			/* R14 (LR)                                               */
    /// ??????????????????????
    *--stk_pt = (INT32U)0x12121212u;                        /* R12                                                    */
    *--stk_pt = (INT32U)0x03030303u;                        /* R3                                                     */
    *--stk_pt = (INT32U)0x02020202u;                        /* R2                                                     */
	*--stk_pt = (INT32U)(NUMBER_OF_STACKED_BYTES / 10);		/* R1                                                     */
    *--stk_pt = (INT32U)0;                              	/* R0 : argument                                          */
                                                            /* Remaining registers saved on process stack             */
    *--stk_pt = (INT32U)0x07070707u;                        /* R7                                                     */
    *--stk_pt = (INT32U)0x06060606u;                        /* R6                                                     */
    *--stk_pt = (INT32U)0x05050505u;                        /* R5                                                     */
    *--stk_pt = (INT32U)0x04040404u;                        /* R4                                                     */
    *--stk_pt = (INT32U)0x11111111u;                        /* R11                                                    */
    *--stk_pt = (INT32U)0x10101010u;                        /* R10                                                    */
    *--stk_pt = (INT32U)0x09090909u;                        /* R9                                                     */
    *--stk_pt = (INT32U)0x08080808u;                        /* R8                                                     */
}





#if (NESTING_INT == 1)

INT32U OS_CPU_SR_Save(void)
{  
	__asm
	( 
		    "MRS     R0, PRIMASK         \n"
		    "CPSID   I					 \n"
		    "BX      LR					 \n" 
	);
	return 0;
}


void OS_CPU_SR_Restore(INT32U prio)
{  
	__asm
	( 
		    "MSR     PRIMASK, R0         \n"
		    "BX      LR					 \n" 
	);
}

  

#endif


inline void CriticalIncNesting(void)
{
	UserEnterCritical();
	iNesting++;
	UserExitCritical();
}


inline void CriticalDecNesting(void)
{
	UserEnterCritical();
	iNesting--;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
