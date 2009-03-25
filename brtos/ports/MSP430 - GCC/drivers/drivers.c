#include "hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "drivers.h"
#include "BRTOS.h"
#include "event.h"
#include "queue.h"


// driver da porta serial

INT8U receive_byte = 0;

// Declara um ponteiro para o bloco de controle da porta serial
BRTOS_Mutex *SerialResource;

interrupt (USCIAB0RX_VECTOR) USCI0RX_ISR(void);

//Função para adquirir direito exclusivo a porta serial

void acquireUART(void)
{
  // Aloca o recurso da porta serial
  OSMutexPend (SerialResource, 0);
}

//Função para liberar a porta serial
void releaseUART(void)
{
  // Libera o recurso da porta serial
  OSMutexPost(SerialResource);
}


	
void UART_init(INT8U priority){
	// UART 0 setup
	P3SEL |= 0x30;                            // P3.4,5 = USCI_A0 TXD/RXD
	UCA0CTL1 = UCSSEL_2;                      // SMCLK
	UCA0BR0 = 0x41;                           // 9600 from 8Mhz
	UCA0BR1 = 0x3;
	UCA0MCTL = UCBRS_2;                       
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
	
	// Cria um mutex com contador = 1, informando que o recurso está disponível
  // após a inicialização
  // Prioridade máxima a acessar o recurso = priority
  if (OSMutexCreate(&SerialResource,priority) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  } 
}


/*------------------------------------------------------------------------------
*
------------------------------------------------------------------------------*/

/**
 * Write character to the serial port. printf and other functions use this to
 * do the actual output.
 */
 
int putchar(int character) {
    while (!(IFG2&UCA0TXIFG));               // wait util tx buffer is free
    UCA0TXBUF = character;                         // send character
    return 1;
}

void Serial_Envia_Caracter(INT8U caracter)
{
  
  while (!(IFG2&UCA0TXIFG));
  UCA0TXBUF = caracter;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
  
}

void Serial_Envia_Frase(char *string)
{

  while(*string)
  { 
    while (!(IFG2&UCA0TXIFG));
	UCA0TXBUF = *string;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
    string++;
  }

}


/*------------------------------------------------------------------------------
* USCIA interrupt service routine
------------------------------------------------------------------------------*/
interrupt (USCIAB0RX_VECTOR) USCI0RX_ISR(void)
{
  extern OS_QUEUE SerialPortBuffer;

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  receive_byte = UCA0RXBUF;;                /* Leitura dos dados recebidos */
  
  if (OSWQueue(&SerialPortBuffer,receive_byte) == BUFFER_UNDERRUN)
  {
    // Problema: Estouro de buffer
    OSCleanQueue(&SerialPortBuffer);
  }
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
  // ************************
}
