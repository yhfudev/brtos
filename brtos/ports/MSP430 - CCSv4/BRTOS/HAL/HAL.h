/**
* \file HAL.h
* \brief BRTOS Hardware Abstraction Layer defines and inline assembly
*
* This file contain the defines and inline assembly that are processor dependent.
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
*                                     OS HAL Header to MSP430
*
*   Authors:  Carlos Henrique Barriquelo
*   Revision: 1.4
*   Date:     19/10/2010
*
*********************************************************************************************************/

#ifndef OS_HAL_H
#define OS_HAL_H

#include "../includes/OS_types.h"
#include "hardware.h"



/// Define if nesting interrupt is active
#define NESTING_INT 0
// Define processor
#define MSP430 1                ///< Enables or disables the MSP430 core as the default processor


extern INT8U iNesting;
extern INT8U currentTask;
extern INT16U SPvalue;          ///< Used to save and restore a task stack pointer


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Port Defines                                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/// Defines the change context command of the chosen processor
#define ChangeContext() SwitchContext()


/// Defines the disable interrupts command of the chosen microcontroller
#define UserEnterCritical() asm("	DINT	");
/// Defines the enable interrupts command of the choosen microcontroller
#define UserExitCritical()  asm("	EINT	");

#if (NESTING_INT == 0)
/// Defines the disable interrupts command of the chosen microcontroller
#define OSEnterCritical() UserEnterCritical()
/// Defines the enable interrupts command of the chosen microcontroller
#define OSExitCritical() UserExitCritical()
#endif

/// Defines the low power command of the chosen microcontroller
#define OS_Wait __bis_SR_register(CPUOFF + GIE)
/// Defines the tick timer interrupt handler code (clear flag) of the chosen microcontroller
#define TICKTIMER_INT_HANDLER 	TACTL |= TACLR
#define TIMER_MODULE 			TACCR0 
#define TIMER_COUNTER 			TAR
#define IDLE_STACK_SIZE 		32


/* stacked by the RTI interrupt process */
// Mínimo de 60 devido ao salvamento de 12 registradores de 16 bits da CPU
// R4-R15 (24 bytes) + SR (12 bits) + SP (20 bits)
#define NUMBER_MIN_OF_STACKED_BYTES 28
/// Minimum size of a task stack.

// User defined: stacked for user function calls + local variables
// Ainda, como podem ocorrer interrupções durante as tarefas, alocar 26 bytes a cada
// interrupção ativa
// 4 bytes to Local Variable Allocation
// 4 bytes to Function Call





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Functions Prototypes                        /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority)
* \brief Install a task. Initial state = running.
* \param *FctPtr Pointer to the task to be installed
* \param *TaskName Task Name or task description
* \param USER_STACKED_BYTES Size of the task virtual stack. Depends on the user code and used interrupts.
* \param iPriority Desired task priority
* \return OK Task successfully installed
* \return NO_MEMORY Not enough memory available to install the task
* \return END_OF_AVAILABLE_PRIORITIES All the available priorities are busy
* \return BUSY_PRIORITY Desired priority busy
*********************************************************************************************/
INT8U InstallTask(void(*FctPtr)(void),const CHAR8 *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority);

/*****************************************************************************************//**
* \fn INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES)
* \brief Install the idle task. Initial state = running.
* \param *FctPtr Pointer to the task to be installed
* \param USER_STACKED_BYTES Size of the task virtual stack.
* \return OK Idle task successfully installed
* \return NO_MEMORY Not enough memory available to install the idle task
*********************************************************************************************/
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES);

/*****************************************************************************************//**
* \fn void Idle(void)
* \brief Idle Task. May be used to implement low power commands.
* \return NONE
*********************************************************************************************/
void Idle(void);

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

// fix MSP430 sw isr
/*****************************************************************************************//**
* \fn void SwitchContext(void)
* \brief Switch context function (mimic SW ISR in MSP430)
* \return NONE
*********************************************************************************************/
void SwitchContext(void);



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////          MSP430 Without Nesting Defines             /////
/////                                                     /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Context Define                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#define SaveContext() 			asm("	PUSH.W	R10 \n\t" \
                                    "	PUSH.W	R9 \n\t" \
                                    "	PUSH.W 	R8 \n\t" \
                                    "	PUSH.W	R7 \n\t" \
                                    "	PUSH.W	R6 \n\t" \
                                    "	PUSH.W	R5 \n\t" \
                                    "	PUSH.W	R4 \n\t" )


#define OS_SAVE_CONTEXT() SaveContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Stack Pointer Define                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

// save top of stack
#define SaveCurrentSP() 		asm("	MOV.W	R1,	&SPvalue \n\t") 

#define OS_SAVE_CURRENT_SP() SaveCurrentSP()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Restore Context Define                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
// restore top of stack
// restore other CPU registers
// adjust stack pointer value
#define RestoreContext() 		asm("	MOV.W	SPvalue,	R1 \n\t" \
									"	POP.W	R4 \n\t" \
                                    "	POP.W	R5 \n\t" \
                                    "	POP.W	R6 \n\t" \
                                    "	POP.W	R7 \n\t" \
                                    "	POP.W	R8 \n\t" \
									"	POP.W	R9 \n\t" \
									"	POP.W	R10 \n\t")

#define OS_RESTORE_CONTEXT() RestoreContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn OS_SAVE_ISR() and OS_RESTORE_ISR()
* \brief Used to mimic ISR call
* \return NONE
*********************************************************************************************/
#define OS_SAVE_ISR()            asm("	PUSH.W	R2 \n\t" \
                                     "	PUSH.W	R15 \n\t" \
                                     "	PUSH.W	R14 \n\t" \
                                     "	PUSH.W	R13 \n\t" \
                                     "	PUSH.W	R12 \n\t" \
                                     "	PUSH.W	R11 \n\t")
									 
#define OS_RESTORE_ISR() 		 asm("	POP.W	R11 \n\t" \
                                     "	POP.W	R12 \n\t" \
                                     "	POP.W	R13 \n\t" \
                                     "	POP.W	R14 \n\t" \
                                     "	POP.W	R15 \n\t" \
									 "	BIC	#240,	0(R1)\n\t" \
									 "	POP.W	R2 \n\t")
									 
/*********************************************************************************************/									 
      	
/// Save Context Define


#define OS_INT_ENTER()		iNesting++;	OS_SAVE_CONTEXT(); OS_SAVE_CURRENT_SP()
	

	      	
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

  #define OS_INT_EXIT()  iNesting--;  if (!iNesting)  { OSSchedule();} OS_RESTORE_CONTEXT()    
  
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



#endif
