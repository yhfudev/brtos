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


/// Define if nesting interrupt is active
#define NESTING_INT 1


extern INT8U iNesting;
extern INT8U currentTask;

// Define if Coldfire
#define Coldfire 1              ///< Enables or disables the coldfire core as the default processor
#define HCS08 0                 ///< Enables or disables the HCS08 core as the default processor

#if HCS08 == 1
  extern INT16U SPvalue;          ///< Used to save and restore a task stack pointer
#else
  extern INT32U SPvalue;          ///< Used to save and restore a task stack pointer
#endif



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Port Defines                                /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/// Defines the change context command of the choosen processor
#define ChangeContext(void) asm ("TRAP %14");


/// Defines the disable interrupts command of the choosen microcontroller
#define UserEnterCritical() DisableInterrupts;
/// Defines the enable interrupts command of the choosen microcontroller
#define UserExitCritical() EnableInterrupts;

#if (NESTING_INT == 0)
/// Defines the disable interrupts command of the choosen microcontroller
#define OSEnterCritical() UserEnterCritical()
/// Defines the enable interrupts command of the choosen microcontroller
#define OSExitCritical() UserExitCritical()
#endif

/// Defines the low power command of the choosen microcontroller
#define OS_Wait asm(STOP #0x2000);
/// Defines the tick timer interrupt handler code (clear flag) of the choosen microcontroller
#define TICKTIMER_INT_HANDLER TPM1SC_TOF = 0
#define TIMER_MODULE  TPM1MOD
#define TIMER_COUNTER TPM1CNT


// stacked by the RTI interrupt process
// Mínimo de 60 devido ao salvamento de 15 registradores de 32 bits da CPU
// D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes)
/// Minimum size of a task stack.
/// Example in the coldfire processor: D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes) = 68 bytes
#if (NESTING_INT == 1)
#define NUMBER_MIN_OF_STACKED_BYTES 72
#else
#define NUMBER_MIN_OF_STACKED_BYTES 68
#endif

// User defined: stacked for user function calls + local variables
// Ainda, como podem ocorrer interrupções durante as tarefas, alocar 28 bytes a cada
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

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////               Coldfire Nesting Defines              /////
/////                                                     /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

#if (NESTING_INT == 1)
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Context Define                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void SaveContext(void)
* \brief Save context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void SaveContext2(void) 
{  
	LEA		   -36(A7),A7				    // reserve space on current stack
	MOVEM.L  D3-D7/A2-A6,(A7)			// save CPU registers
}

/// Save Context Define
#define OS_SAVE_CONTEXT2() SaveContext2()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




/*****************************************************************************************//**
* \fn inline asm void SaveContext(void)
* \brief Save context function
* \return NONE
*********************************************************************************************/

inline asm __declspec(register_abi) void OSIntEnter(void) 
{  
      MVZ.B    iNesting,D0          // incrementa iNesting
      ADDQ.L   #1,D0
      MOVE.B   D0,iNesting
      
      MOV3Q    #1,D1                // Verifica se é a primeira interrupção
      CMP.B    D0,D1                // se for, salva stack pointer
      BNE.S    SAVE_NO_CONTEXT
	    
	    LEA		   -36(A7),A7				    // save context and stack pointer
	    MOVEM.L  D3-D7/A2-A6,(A7)     
      MOVE.L   A7,SPvalue
SAVE_NO_CONTEXT:
}


/// Save Context Define
#define OS_INT_ENTER() OSIntEnter()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////




/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void RestoreSR(void)
{  
    MOVE.W   SR,D1
    MOVE.W   62(A7),D0
    ANDI.L   #0x0700,D0    
    ANDI.L   #0xF8FF,D1
    OR       D1,D0
    MOVE.W   D0,SR
}

/// Restore Context Define
#define OS_ENABLE_NESTING() RestoreSR()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Restore Context Define                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void RestoreContext(void)
{  	
	MOVEM.L (A7),D3-D7/A2-A6  			// restore other CPU registers
	LEA		  36(A7),A7			     	    // adjust stack pointer value
}

/// Restore Context Define
#define OS_RESTORE_CONTEXT() RestoreContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Restore Context Define                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void RestoreSP(void)
{  
	MOVE    SPvalue,A7              // restore top of stack
}

/// Restore Context Define
#define OS_RESTORE_SP() RestoreSP()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





INT16U OS_CPU_SR_Save(void);


#define  OSEnterCritical()  (CPU_SR = OS_CPU_SR_Save())	 // Disable interrupts    


void OS_CPU_SR_Restore(INT16U);


#define  OSExitCritical()  (OS_CPU_SR_Restore(CPU_SR))	 // Enable interrupts  


void CriticalDecNesting(void);

// Change context only if Nesting = 0
// i.e. if there are no interrupt nesting

#define OS_INT_EXIT()                                                   \
  CriticalDecNesting();                                                 \
  if (!iNesting)                                                        \
  {                                                                     \
     OSSchedule();                                                      \
     OS_RESTORE_SP();                                                   \
     OS_RESTORE_CONTEXT();                                              \
  }else                                                                 \
  {                                                                     \
    asm                                                                 \
    {                                                                   \
          MOVE.W   D2,SR                                                \
    }                                                                   \
  }                                                                     \
  
#else










///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/////                                                     /////
/////          Coldfire Without Nesting Defines           /////
/////                                                     /////
/////                                                     /////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Context Define                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

inline asm __declspec(register_abi) void SaveContext(void) 
{  
	LEA		   -40(A7),A7				    // reserve space on current stack
	MOVEM.L  D3-D7/A2-A6,(A7)			// save CPU registers
}

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

inline asm __declspec(register_abi) void SaveCurrentSP(void) 
{  
	MOVE     A7,SPvalue           // save top of stack
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Restore Context Define                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

inline asm __declspec(register_abi) void RestoreContext(void)
{  
	MOVE    SPvalue,A7              // restore top of stack
	MOVEM.L (A7),D3-D7/A2-A6  			// restore other CPU registers
	LEA		  40(A7),A7			     	    // adjust stack pointer value
}

#define OS_RESTORE_CONTEXT() RestoreContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


/*****************************************************************************************//**
* \fn inline asm void SaveContext(void)
* \brief Save context function
* \return NONE
*********************************************************************************************/

inline asm __declspec(register_abi) void OSIntEnter(void) 
{  
      MVZ.B    iNesting,D0          // incrementa iNesting
      ADDQ.L   #1,D0
      MOVE.B   D0,iNesting
	    
	    LEA		   -40(A7),A7				    // save context and stack pointer
	    MOVEM.L  D3-D7/A2-A6,(A7)     
      MOVE.L   A7,SPvalue
}


/// Save Context Define
#define OS_INT_ENTER() OSIntEnter()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#define OS_INT_EXIT()                                                   \
  iNesting--;                                                           \
  if (!iNesting)                                                        \
  {                                                                     \
     OSSchedule();                                                      \
     OS_RESTORE_CONTEXT();                                              \
  }                                                                     \
  
#endif    
  
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



#endif
