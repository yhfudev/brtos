/**
* \file HAL.h
* \brief BRTOS Hardware Abstraction Layer defines and inline assembly
*
* This file contain the defines and inline assembly that are processor dependant.
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
*                                     OS HAL Header to Coldfire V1
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*   Authors:  Carlos Henrique Barriquelo e Gustavo Weber Denardin
*   Revision: 1.2
*   Date:     01/10/2010
*
*********************************************************************************************************/

#ifndef OS_HAL_H
#define OS_HAL_H

#include "OS_types.h"

/// Supported processors
#define COLDFIRE_V1		1u
#define HCS08			2u
#define MSP430			3u
#define ATMEGA			4u
#define PIC18			5u
#define RX600			6u
#define ARM_Cortex_M3	7u
#define ARM_Cortex_M4	8u


/// Define the used processor
#define PROCESSOR 		ARM_Cortex_M4

/// Define the CPU type
#define OS_CPU_TYPE 	INT32U

/// Define if the optimized scheduler will be used
#define OPTIMIZED_SCHEDULER 1

/// Define if InstallTask function will support parameters
#define TASK_WITH_PARAMETERS 0

/// Define if 32 bits register for tick timer will be used
#define TICK_TIMER_32BITS   1

/// Define if nesting interrupt is active
#define NESTING_INT 1

/// Define if its necessary to save status register / interrupt info
#define OS_SR_SAVE_VAR INT32U CPU_SR = 0;

/// Define stack growth direction
#define STACK_GROWTH 0            /// 1 -> down; 0-> up

/// Define CPU Stack Pointer Size
#define SP_SIZE 32

#define READY_LIST_VAR read_list


extern INT8U iNesting;
extern volatile INT32U SPvalue;




/* Constants required to set up the initial stack. */
#define INITIAL_XPSR			0x01000000

/* Constants required to manipulate the NVIC. */
#define NVIC_PENDSVSET      	0x10000000         						// Value to trigger PendSV exception.
#define NVIC_SYSTICK_CTRL       ( ( volatile unsigned long *) 0xe000e010 )
#define NVIC_SYSTICK_LOAD       ( ( volatile unsigned long *) 0xe000e014 )
#define NVIC_INT_CTRL           ( ( volatile unsigned long *) 0xe000ed04 )	// Interrupt control state register.

#define NVIC_SYSTICK_CLK        0x00000004
#define NVIC_SYSTICK_INT        0x00000002
#define NVIC_SYSTICK_ENABLE     0x00000001

#define SCB_CPACR				((volatile unsigned long *)0xE000ED88)
#define FPU_FPCCR				((volatile unsigned long *)0xE000EF34)




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Port Defines                                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void ChangeContext(void);
								   

INT32U OS_CPU_SR_Save(void);
#define  OSEnterCritical()  (CPU_SR = OS_CPU_SR_Save())	 // Disable interrupts    
void OS_CPU_SR_Restore(INT32U);
#define  OSExitCritical()  (OS_CPU_SR_Restore(CPU_SR))	 // Enable interrupts

/// Defines the disable interrupts command of the choosen microcontroller
#define UserEnterCritical() __asm(" CPSID I")
/// Defines the enable interrupts command of the choosen microcontroller
#define UserExitCritical()  __asm(" CPSIE I")

/// Defines the low power command of the choosen microcontroller
#define OS_Wait __asm(" WFI ");
/// Defines the tick timer interrupt handler code (clear flag) of the choosen microcontroller
#define TICKTIMER_INT_HANDLER
#define TIMER_MODULE	SYST_RVR
#define TIMER_COUNTER	SYST_CVR


// stacked by the RTI interrupt process
#define NUMBER_MIN_OF_STACKED_BYTES 64




////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Functions Prototypes                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (TASK_WITH_PARAMETERS == 1)
  void CreateVirtualStack(void(*FctPtr)(void*), INT16U NUMBER_OF_STACKED_BYTES, void *parameters);
#else
  void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES);
#endif


/*****************************************************************************************//**
* \fn void TickTimerSetup(void)
* \brief Tick timer clock setup
* \return NONE
*********************************************************************************************/
void TickTimerSetup(void);

/*****************************************************************************************//**
* \fn void OSRTCSetup(void)
* \brief Real time clock setup
* \return NONE
*********************************************************************************************/
void OSRTCSetup(void);

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




/*****************************************************************************************//**
* \fn inline asm void SaveContext(void)
* \brief Save context function
* \return NONE
*********************************************************************************************/
/// Save Context Define
#define OS_SAVE_CONTEXT() __asm(	" MRS     R0, PSP		\n"		\
							  		" SUBS    R0, R0, #0x20	\n"		\
							  		" STM     R0, {R4-R11}	\n"		\
						  )
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// Restore Context Define
#define OS_RESTORE_CONTEXT() __asm(														\
								  /* Restore r4-11 from new process stack */			\
								  " LDM     R0, {R4-R11}		\n"						\
								  " ADDS    R0, R0, #0x20		\n"						\
								  /* Load PSP with new process SP */					\
								  " MSR     PSP, R0				\n"						\
								  )
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn inline asm void SaveSP(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// Save Stack Pointer Define
#define OS_SAVE_SP() 	__asm(" PUSH {R0} ");			\
						SPvalue = (INT32U)&SPvalue;		\
						__asm(" POP {R0} ");			\
						__asm(" STR   R0, [R1] ")


////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/

#define OS_RESTORE_SP()	(void)SPvalue							 

////////////////////////////////////////////////////////////



/*****************************************************************************************//**
* \fn inline asm void RestoreSR(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// No ARM o nesting estará habilitado por padrão
/// Restore Status Register Define
#define OS_ENABLE_NESTING()
////////////////////////////////////////////////////////////


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




#define BTOSStartFirstTask() __asm(	" cpsie i				\n"					\
									" svc #0				\n"					\
								  )
	   

#define OS_RESTORE_ISR() __asm(	  /* Ensure exception return uses process stack */		\
								  " ORR     LR, LR, #0x04	\n"							\
								  " CPSIE   I				\n"							\
								  /* Exception return will restore remaining context */ \
								  " BX      LR               \n"							\
								  )





#if (OPTIMIZED_SCHEDULER == 1)

#define Optimezed_Scheduler()	__asm(	" CLZ     R0, R0		\n"		\
  										" RSB     R0, R0, #0x1F	\n"		\
									 )

#endif

#endif
