#ifndef OS_HAL_H
#define OS_HAL_H

#include "OS_types.h"

extern INT16U SPvalue;

#define Coldfire 0
#define HCS08 1



// *******************************************************
// * Port Defines                                        *
// *******************************************************
#define ChangeContext() asm ("SWI");
#define OSEnterCritical asm ("SEI");
#define OSExitCritical asm ("CLI");

extern INT8U maxPriority;


//Stack Defines

/* stacked by the RTI interrupt process */
// Mínimo de 60 devido ao salvamento de 15 registradores de 32 bits da CPU
// D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes)
#define NUMBER_MIN_OF_STACKED_BYTES 6

/* User defined: stacked for user function calls + local variables */
// Ainda, como podem ocorrer interrupções durante as tarefas, alocar 28 bytes a cada
// interrupção ativa
// 4 bytes to Local Variable Allocation
// 4 bytes to Function Call



INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority);
INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES);
void Idle(void);
void TickTimerSetup(void);


#define OS_SAVE_CONTEXT() \
{                         \
   asm("PULH");           \
 }                         



#define SaveCurrentSP() \
{                       \
  asm("TSX");           \
  asm("STHX SPvalue");  \
}



#define OS_RESTORE_CONTEXT() \
{                            \
  asm("LDHX SPvalue");       \
  asm("TXS");                \
}



#endif