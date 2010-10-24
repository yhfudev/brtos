/**
* \file drivers.c
* \brief Microcontroller drivers
*
* This file contain the main microcontroller drivers: A/D, PWM, serial and keyboard.
*
**/


#include "../includes/drivers.h"
#include "../includes/BRTOS.h"

// Declara um ponteiro para o bloco de controle da porta serial
BRTOS_Mutex *SerialResource;
extern BRTOS_Queue *Serial;

INT16U SPvalue;

INT8U receive_byte;

//Fun��o para adquirir direito exclusivo a porta serial
// Assim que poss�vel colocar recurso de timeout
void acquireUART(void)
{
  // Aloca o recurso da porta serial
  OSMutexAcquire(SerialResource);
}

//Fun��o para liberar a porta serial
void releaseUART(void)
{
  // Libera o recurso da porta serial
  OSMutexRelease(SerialResource);
}


void Serial_Envia_Caracter(CHAR8 caracter)
{
  while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
  UCA0TXBUF = caracter;  	/* Armazena o caracter a ser transmitido no registrador de transmiss�o */
                
}

void Serial_Envia_Frase(CHAR8 *string)
{

  while(*string)
  { 
  	while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = *string;                   /* Armazena o caracter a ser transmitido no registrador de transmiss�o */
    string++;
  }

}
/*------------------------------------------------------------------------------
* USCIA interrupt service routine
------------------------------------------------------------------------------*/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  // ************************
  // Entrada de interrup��o
  // ************************
  //OS_INT_ENTER();
  iNesting++;  
  OS_SAVE_CONTEXT();
  SaveCurrentSP();

  // Tratamento da interrup��o
  receive_byte = UCA0RXBUF;   /* Leitura dos dados recebidos */   
  
  if (OSQueuePost(Serial, receive_byte) == BUFFER_UNDERRUN)
  {
    // Problema: Estouro de buffer
    OSCleanQueue(Serial);
  }
  
  // ************************
  // Interrupt Exit
  // ************************
  //OS_INT_EXIT();  
  // ************************
  iNesting--;                                                           
  if (!iNesting)                                                        
  {                                                                     
     OSSchedule();                                 
  } 
  
  OS_RESTORE_CONTEXT();  
}


void init_UART(INT8U priority)
{

  P3SEL |= 0x30;                            // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 = UCSSEL_2;                      // SMCLK
  UCA0BR0 = 0x83;                           // 9600 from 16Mhz
  UCA0BR1 = 0x6;
  UCA0MCTL = UCBRS_2;                       
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt   
  
  // Cria um mutex com contador = 1, informando que o recurso est� dispon�vel
  // ap�s a inicializa��o
  // Prioridade m�xima a acessar o recurso = priority
  if (OSMutexCreate(&SerialResource,priority) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // N�o deveria entrar aqui !!!
    while(1){};
  }  
}


