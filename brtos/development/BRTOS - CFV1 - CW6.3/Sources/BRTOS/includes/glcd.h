/**
* \file glcd.h
* \brief Graphic LCD main functions prototypes.
*
*
**/

void glcd_init(byte mode);
void glcd_pixel(byte x, byte y, byte color);
void glcd_fillScreen(byte color);
void glcd_fillside(byte color, byte side);
void glcd_writeByte(byte side, byte data);
byte glcd_readByte(byte side);
void bit_set(byte dado2, byte bit2);
void bit_clear(byte dado2, byte bit2);
void teste_linha(byte x, byte y);
void teste_nome(byte x, byte y, byte num, const unsigned char nomes[]);
void delay(unsigned int tempo);