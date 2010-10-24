/**
* \file drivers.h
* \brief Microcontroller drivers defines and function prototypes.
*
*
**/

#include "../includes/BRTOS.h"
#include "../HAL/HAL.h"

/* UART functions prototypes */
void acquireUART(void);
void releaseUART(void);
void init_UART(INT8U);

void Serial_Envia_Caracter(CHAR8 caracter);
void Serial_Envia_Frase(CHAR8 *string);

////////////////////////////////////////////////
////////////////////////////////////////////////
///    Definições do Módulo Serial           ///
////////////////////////////////////////////////
////////////////////////////////////////////////
#define CR             13         //  ASCII code for carry return
#define LF             10         //  ASCII code for line feed

