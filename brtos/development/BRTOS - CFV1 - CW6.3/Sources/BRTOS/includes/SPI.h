/**
* \file SPI.h
* \brief Serial peripheral interface driver function prototypes.
*
*
**/

void init_SPI1(void);
void SPISendChar (INT8U data);
INT8U SPIGet(void);