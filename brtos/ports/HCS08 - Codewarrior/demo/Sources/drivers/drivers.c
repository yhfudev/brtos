#include "derivative.h" /* include peripheral declarations */
#include "BRTOS.h"

void init_TPM2(void)
{  
  /* ### Init_TPM init code */
  /* TPM2SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  TPM2SC = 0x00;                       /* Stop and reset counter */
  TPM2MOD = 60000;                     /* Period value setting */
  (void)(TPM2SC == 0);                 /* Overflow int. flag clearing (first part) */
  /* TPM2SC: TOF=0,TOIE=1,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
  TPM2SC = 0x48;                       /* Int. flag clearing (2nd part) and timer control register setting */
}



// Interrupt Code Example
// In HCS08 microcontroller there are no need to save context.
// H register is the only register to be saved.
// C compiler auto save the H register to interrupt service routines

interrupt void TPM2Over(void)
{
  // ************************
  // Entrada de interrupção
  // ************************

  iNesting++;
  
  SaveCurrentSP();
  // ************************  

  // Tratamento da interrupção
  TPM2SC_TOF = 0;
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();  
  
// ************************
}
