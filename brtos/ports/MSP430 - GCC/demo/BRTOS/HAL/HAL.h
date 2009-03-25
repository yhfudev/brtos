
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
*                                           MSP430 port functions - MSPGCC compiler
*
*
*   Author:   Carlos H. Barriquello
*   Revision: 1.0
*   Date:     24/03/2009
*
*********************************************************************************************************/


#ifndef OS_HAL_H
#define OS_HAL_H

#include "OS_types.h"

extern INT16U SPvalue;

#define MSP430 1



// *******************************************************
// * Port Defines                                        *
// *******************************************************
#define ChangeContext() SwitchContext()

#define OSEnterCritical dint()
#define OSExitCritical eint()


/* stacked by the RTI interrupt process */
// Mínimo de 60 devido ao salvamento de 12 registradores de 16 bits da CPU
// R4-R15 (24 bytes) + SR (12 bits) + SP (20 bits)
#define NUMBER_MIN_OF_STACKED_BYTES 28

/* User defined: stacked for user function calls + local variables */
// Ainda, como podem ocorrer interrupções durante as tarefas, alocar 28 bytes a cada
// interrupção ativa
// 4 bytes to Local Variable Allocation
// 4 bytes to Function Call

INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority);
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES);
void Idle(void);
void OSRTCSetup(void);

// fix MSP430 sw isr
void SwitchContext(void);


void TickTimerSetup(void);
void PreInstallTasks(void);
#define SaveContext() asm volatile ( "PUSH.W R10 \n\t" \
                                     "PUSH.W R9 \n\t" \
                                     "PUSH.W R8 \n\t" \
                                     "PUSH.W R7 \n\t" \
                                     "PUSH.W R6 \n\t" \
                                     "PUSH.W R5 \n\t" \
                                     "PUSH.W R4 \n\t" )


#define OS_SAVE_CONTEXT() SaveContext()

// save top of stack
#define SaveCurrentSP() asm("mov.w r1,&SPvalue \n\t") 

// restore top of stack
// restore other CPU registers
// adjust stack pointer value
#define RestoreContext() asm volatile( 	"mov.w SPvalue,r1 \n\t" \
										"POP.W R4 \n\t" \
                                        "POP.W R5 \n\t" \
                                        "POP.W R6 \n\t" \
                                        "POP.W R7 \n\t" \
                                        "POP.W R8 \n\t" \
										"POP.W R9 \n\t" \
										"POP.W R10 \n\t")

#define OS_RESTORE_CONTEXT() RestoreContext()

#define OS_SAVE_ISR() asm volatile ( "push.w r2 \n\t" \
                                     "PUSH.W R15 \n\t" \
                                     "PUSH.W R14 \n\t" \
                                     "PUSH.W R13 \n\t" \
                                     "PUSH.W R12 \n\t" \
                                     "PUSH.W R11 \n\t")
									 
#define OS_RESTORE_ISR() asm volatile ( "POP.W R11 \n\t" \
                                        "POP.W R12 \n\t" \
                                        "POP.W R13 \n\t" \
                                        "POP.W R14 \n\t" \
                                        "POP.W R15 \n\t" \
										"bic	#240,	0(r1)" \
										"pop.w r2 \n\t" \
										"pop.w r2 \n\t")


#endif



