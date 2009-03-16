#ifndef OS_HAL_H
#define OS_HAL_H

#include "types.h"

extern INT32U SPvalue;
extern INT32U SPBackup;
extern INT16U SRBackup;

// Define if Coldfire
#define Coldfire 1



// *******************************************************
// * Port Defines                                        *
// *******************************************************
#define ChangeContext(void) asm ("TRAP %14");

#define OSEnterCritical DisableInterrupts;
#define OSExitCritical EnableInterrupts;



/* stacked by the RTI interrupt process */
// Mínimo de 60 devido ao salvamento de 15 registradores de 32 bits da CPU
// D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes)
#define NUMBER_MIN_OF_STACKED_BYTES 68

/* User defined: stacked for user function calls + local variables */
// Ainda, como podem ocorrer interrupções durante as tarefas, alocar 28 bytes a cada
// interrupção ativa
// 4 bytes to Local Variable Allocation
// 4 bytes to Function Call

INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority);
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES);
void Idle(void);
void OSRTCSetup(void);


inline asm void SaveContext(void) 
{  
	LEA		   -40(A7),A7				    // reserve space on current stack
	MOVEM.L  D3-D7/A2-A6,(A7)			// save CPU registers
}

#define OS_SAVE_CONTEXT() SaveContext()

inline asm void SaveCurrentSP(void) 
{  
	MOVE     A7,SPvalue           // save top of stack
}

inline asm void RestoreContext(void)
{  
	MOVE    SPvalue,A7              // restore top of stack
	MOVEM.L (A7),D3-D7/A2-A6  			// restore other CPU registers
	LEA		  40(A7),A7			     	    // adjust stack pointer value
}

#define OS_RESTORE_CONTEXT() RestoreContext()


inline asm void ExamineSP(void)
{ 
  // 64 pq está dentro de uma função
  MOVE.W 66(A7),D0
  ANDI.L #0x0700,D0
  MOVE   D0,SPBackup
}


inline asm void FixSR(void)
{ 
  // 64 pq está dentro de uma função
  MOVE.W #0x2000,66(A7)
}


#endif