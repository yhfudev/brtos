#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "drivers.h"
#include "myRTOS.h"
#include "event.h"
#include "queue.h"

#define radio_CS PTCD_PTCD0
#define PHY_CS   PTED_PTED3

INT8U SPIData = 0;

void init_SPI1(void)
{
  PTEDD = 8;
  //PTEPE = 7;
  PTED = 8;
  PHY_CS = 1;
  
  /* ### Init_SPI init code */
  (void)SPI1S;                         /* Read the status register */
  (void)SPI1D;                         /* Read the device register */
  /* SPI1C1: SPIE=0,SPE=0,SPTIE=0,MSTR=0,CPOL=0,CPHA=1,SSOE=0,LSBFE=0 */
  SPI1C1 = 0x00;                       /* The SPRF interrupt flag is cleared when the SPI1 module is disabled. */
  
  /* SPI1C2: MODFEN=0,BIDIROE=0,SPISWAI=0,SPC0=0 */
  // Modo bidirecional single-wire desativado
  SPI1C2 = 0x00;                                      
  
  /* SPI1BR: SPPR2=0,SPPR1=0,SPPR0=1,SPR2=0,SPR1=0,SPR0=0 */
  // Configura o clock da porta SPI p/ 6 Mhz --> 24Mhz / 2*2
  //SPI1BR = 0x70;
  SPI1BR = 0x10;
                                        
  (void)(SPI1S == 0);                  /* Dummy read of the SPI1S registr to clear the MODF flag */
  
  /* SPI1C1: SPIE=0,SPE=0,SPTIE=0,MSTR=1,CPOL=0,CPHA=0,SSOE=0,LSBFE=0 */
  SPI1C1 = 0x90;
  SPI1C1_SPE = 1;
  /* ### */
}



interrupt void SPI1(void)
{ 
  // Write your interrupt code here ... 

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção

  //while (PTED_PTED0){};                 // wait for clock to return no default
  SPI1S;                                  // Acknowledge flag
  SPIData = SPI1D;                        // Acknowledge flag
  
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}



// Função para enviar dados pela porta SPI
void SPISendChar (INT8U data){
  while (!SPI1S_SPTEF){};               /* wait until transmit buffer is empty*/
  (void)SPI1S;
  SPI1D = data;                         /* Transmit counter*/  
}


INT8U SPIGet(void)
{
    int i = 0;
    
    SPISendChar(0x00);
    
    for(i=0;i<40;i++){};       
    return SPIData;
}