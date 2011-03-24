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
#include "derivative.h" /* include peripheral declarations */

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

/// Define if nesting interrupt is active
#define NESTING_INT 1

/// Define the Reset Watchdog macro
#define RESET_WATCHDOG()		__asm(CPSID I);			\
								WDOG_REFRESH = 0xA602;	\
								WDOG_REFRESH = 0xB480;	\
								__asm(CPSIE I)

/// Define if its necessary to save status register / interrupt info
#define OS_SR_SAVE_VAR INT32U CPU_SR = 0;

/// Define stack growth direction
#define STACK_GROWTH 0            /// 1 -> down; 0-> up

/// Define CPU Stack Pointer Size
#define SP_SIZE 32


extern INT8U iNesting;
extern INT32U SPvalue;



/* Constants required to set up the initial stack. */
#define INITIAL_XPSR		0x01000000

/* Constants required to manipulate the NVIC. */
#define NVIC_INT_CTRL		0xE000ED04								// Interrupt control state register.
#define NVIC_SYSPRI14       0xE000ED22         						// System priority register (priority 14).
#define NVIC_PENDSV_PRI     0xFF        	 						// PendSV priority value (lowest).
#define NVIC_PENDSVSET      0x10000000         						// Value to trigger PendSV exception.



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Port Defines                                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/// Defines the change context command of the choosen processor
/* Talvez tenha que desbloquear as interrup��es (As vezes o BRTOS chama est� fun��o
*  com as interrup��es bloqueadas !!!!!
*  Talvez chamar a fun��o OSExitCritical antes para limpar o backup de condi��o cr�tica
*  OS_CPU_SR  
*/
#define ChangeContext(void) __asm(										\
									"LDR     R0, =NVIC_INT_CTRL		\n"	\
									"LDR     R1, =NVIC_PENDSVSET	\n" \
									"STR     R1, [R0]				\n"	\
									"BX      LR						\n"	\
									"CPSIE I						\n" \
								   )

INT32U OS_CPU_SR_Save(void);
#define  OSEnterCritical()  (CPU_SR = OS_CPU_SR_Save())	 // Disable interrupts    
void OS_CPU_SR_Restore(INT32U);
#define  OSExitCritical()  (OS_CPU_SR_Restore(CPU_SR))	 // Enable interrupts

/// Defines the disable interrupts command of the choosen microcontroller
#define UserEnterCritical() __asm(CPSID I)
/// Defines the enable interrupts command of the choosen microcontroller
#define UserExitCritical()  __asm(CPSIE I)

/// Defines the low power command of the choosen microcontroller
#define OS_Wait //__asm(WFI);
/// Defines the tick timer interrupt handler code (clear flag) of the choosen microcontroller
#define TICKTIMER_INT_HANDLER
#define TIMER_MODULE
#define TIMER_COUNTER


// stacked by the RTI interrupt process
#define NUMBER_MIN_OF_STACKED_BYTES 64






////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Functions Prototypes                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void CreateVirtualStack(void(*FctPtr)(void), INT16U NUMBER_OF_STACKED_BYTES);

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
#define OS_SAVE_CONTEXT() __asm(								\
							  "MRS     R0, PSP			\n"		\
							  "SUBS    R0, R0, #0x20	\n"		\
							  "STM     R0, {R4-R11}		\n"		\
						  )
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// Restore Context Define
#define OS_RESTORE_CONTEXT() __asm(											\
								  /* Restore r4-11 from new process stack */\
								  "LDM     R0, {R4-R11}		\n"				\
								  "ADDS    R0, R0, #0x20	\n"				\
								  /* Load PSP with new process SP */		\
								  "MSR     PSP, R0			\n"				\
								  )
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn inline asm void SaveSP(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// Save Stack Pointer Define
#define OS_SAVE_SP() __asm(	"LDR	R1,	=SPvalue	\n"		\
							"STR    R0, [R1]		\n"		\
						  )

////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// Restore Stack Pointer Define
#define OS_RESTORE_SP()	__asm(	"LDR	 R1, =SPvalue	\n"		\
								"LDR     R0, [R1]		\n"		\
							 )
////////////////////////////////////////////////////////////



/*****************************************************************************************//**
* \fn inline asm void RestoreSR(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
/// No ARM o nesting estar� habilitado por padr�o
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


/*
 * Este � o normal do BRTOS, mas n�o serve para o ARM
 * Acredito que este c�digo v� para a interrup��o SVC (usada para iniciar o sistema)
#define BTOSStartFirstTask()	OS_RESTORE_SP();	  \
								OS_RESTORE_CONTEXT(); \
								OS_RESTORE_ISR();
 */
#define BTOSStartFirstTask() __asm( /* Call SVC to start the first task. */		\
									"cpsie i				\n"					\
									"svc 0					\n"					\
								  )

/* Definir prioridade da interrup��o de troca de contexto 
	   
#define OS_RESTORE_INT()	__asm(	"LDR     R0, =NVIC_SYSPRI14		\n"	\
									"LDR     R1, =NVIC_PENDSV_PRI	\n"	\
									"STRB    R1, [R0]				\n"	\

*/

#define OS_RESTORE_ISR() __asm(	  /* Ensure exception return uses process stack */		\
								  "ORR     LR, LR, #0x04	\n"							\
								  "CPSIE   I				\n"							\
								  /* Exception return will restore remaining context */ \
								  "BX      LR               \n"							\
								  )

//#define CHANGE_SP() 	__asm( " PUSH {LR}");											\
						ChangeSP();														\
						__asm( " POP {LR}")
								  
/* Pelo que vi a troca de contexto deve ser realizada dentro de uma exce��o
   No FreeRTOS utilizaram a exce��o SVC para iniciar a primeira task (parece ser a melhor op��o) */


#endif
