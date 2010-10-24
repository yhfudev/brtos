/**
* \file glcd.c
* \brief Graphic LCD main functions.
*
* This file contain the main functions of the graphic lcd, such as:
* initialize the glcd, write a pixel, read a column, fill the screen,etc.
*
**/

#include "hardware.h"
#include "event.h"
#include "BRTOS.h"
#include "graphic.h"
#include "glcd.h"

#pragma warn_implicitconv off



// Declara um ponteiro para o bloco de controle do Display GLCD
BRTOS_Mutex *GLCD;

//Função para adquirir direito exclusivo ao GLCD
// Assim que possível colocar recurso de timeout
void acquireGLCD(void)
{
  // Aloca o recurso do display GLCD
  OSMutexAcquire(GLCD);
}

//Função para liberar o display GLCD
INT8U releaseGLCD(void)
{
  // Libera o recurso do display GLCD
  OSMutexRelease(GLCD);
}



// Propósito:     Inicializar o GLCD.
//                Deve ser chamada antes de qualquer outra função.
// Inputs:        OFF - Desliga o GLCD
//                ON  - Liga o GLCD

void glcd_init(INT8U mode)
{
   // Initialze some pins
   GLCD_RST=1;
   GLCD_E=0;
   GLCD_CS1=0;
   GLCD_CS2=0;
   GLCD_DI=0;                           // Seleciona instrução
   
   glcd_writeByte(GLCD_LEFT,  0xC0);    // Specify first RAM line at the top
   glcd_writeByte(GLCD_RIGHT, 0xC0);    //   of the screen
   glcd_writeByte(GLCD_LEFT,  0x40);    // Set the column address to 0
   glcd_writeByte(GLCD_RIGHT, 0x40);
   glcd_writeByte(GLCD_LEFT,  0xB8);    // Set the page address to 0
   glcd_writeByte(GLCD_RIGHT, 0xB8);

   if(mode == ON)
   {
      glcd_writeByte(GLCD_LEFT,  0x3F); // Turn the display on
      glcd_writeByte(GLCD_RIGHT, 0x3F);
   }
   else
   {
      glcd_writeByte(GLCD_LEFT,  0x3E); // Turn the display off
      glcd_writeByte(GLCD_RIGHT, 0x3E);
   }

   glcd_fillScreen(OFF);                // Clear the display
}


// Purpose:    Fill the LCD screen with the passed in color
// Inputs:     ON  - turn all the pixels on
//             OFF - turn all the pixels off

void glcd_fillScreen(byte color)
{
   int i,j;
   // Loop through the vertical pages
   for(i = 0; i < 8; ++i)
   {
      GLCD_DI=0;                                  // Set for instruction
      glcd_writeByte(GLCD_LEFT, 0b01000000);      // Set horizontal address to 0
      glcd_writeByte(GLCD_RIGHT, 0b01000000);
      glcd_writeByte(GLCD_LEFT, i | 0b10111000);  // Set page address
      glcd_writeByte(GLCD_RIGHT, i | 0b10111000);
      GLCD_DI=1;                                  // Set for data

      // Loop through the horizontal sections
      for(j = 0; j < 64; ++j)
      {
         glcd_writeByte(GLCD_LEFT, 0xFF*color);  // Turn pixels on or off
         glcd_writeByte(GLCD_RIGHT, 0xFF*color); // Turn pixels on or off
      }
   }
}


void glcd_fillside(byte color, byte side)
{
   int i, j;

   // Loop through the vertical pages
   for(i = 0; i < 8; ++i)
   {
      GLCD_DI=0;                                  // Set for instruction
      if (side==GLCD_LEFT){
        
        glcd_writeByte(GLCD_LEFT, 0b01000000);      // Set horizontal address to 0
        glcd_writeByte(GLCD_LEFT, i | 0b10111000);  // Set page address
      } else{ 
        glcd_writeByte(GLCD_RIGHT, 0b01000000);
        glcd_writeByte(GLCD_RIGHT, i | 0b10111000);
      }
      
      GLCD_DI=1;                     // Set for data

      // Loop through the horizontal sections
      for(j = 0; j < 64; ++j)
      {
         if (side==GLCD_LEFT)
          glcd_writeByte(GLCD_LEFT, 0xFF*color);  // Turn pixels on or off
         else
          glcd_writeByte(GLCD_RIGHT, 0xFF*color); // Turn pixels on or off
      }
   }
}


// Purpose:    Write a byte of data to the specified chip
// Inputs:     1) chipSelect - which chip to write the data to
//             2) data - the byte of data to write
void glcd_writeByte(byte side, byte data)
{
  INT16S j = 0;
  
   if(side)                   // Choose which side to write to
      GLCD_CS2=1;
   else
      GLCD_CS1=1;

   GLCD_RW=0;                 // Set for writing
   saida=data;                // Put the data on the port
   
   for (j=8;j>0;j--)
   { 
    asm
    {
      NOP
    }
   }
   
   GLCD_E=1;                 // Pulse the enable pin
   
   
   for(j=20;j>0;j--)
   { 
    asm
    {
      NOP
    }
   }
   
   GLCD_E=0;

   GLCD_CS1=0;               // Reset the chip select lines
   GLCD_CS2=0;
}


// Purpose:    Reads a byte of data from the specified chip
// Ouputs:     A byte of data read from the chip
byte glcd_readByte(byte side)
{
  INT16S j = 0;
  
   byte data;                 // Stores the data read from the LCD

   dir_saida = 0;             // Set saida to input
   GLCD_RW=1;                 // Set for reading

   if(side)                   // Choose which side to write to
      GLCD_CS2=1;
   else
      GLCD_CS1=1;

   for(j=8;j>0;j--)
   { 
    asm
    {
      NOP
    }
   }
   
   
   GLCD_E=1;                   // Pulse the enable pin
   
   for(j=20;j>0;j--)
   { 
    asm volatile
    {
      NOP
    }
   }
   
   data = saida;              // Get the data from the display's output register
   
   for(j=10;j>0;j--)
   { 
    asm volatile
    {
      NOP
    }
   }
      
   GLCD_E=0;
   GLCD_RW=0;                 // Set for writing
   dir_saida = 255;            // Set saida to saida

   GLCD_CS1=0;                // Reset the chip select lines
   GLCD_CS2=0;
   delay(6);
   return data;               // Return the read data
}

void bit_set(byte dado2, byte bit2){
  switch (bit2){
    
  case 0:
    dado2 = dado2 | 1;
    break;
  case 1:
    dado2 = dado2 | 2;
    break;
  case 2:
    dado2 = dado2 | 4;
    break;
  case 3:
    dado2 = dado2 | 8;
    break;
  case 4:
    dado2 = dado2 | 16;
    break;
  case 5:
    dado2 = dado2 | 32;    
    break;
  case 6:
    dado2 = dado2 | 64;
    break;
  case 7:
    dado2 = dado2 | 128;    
    break;
  }
}

void bit_clear(byte dado2, byte bit2){
  switch (bit2){
    
  case 0:
    dado2 = dado2 & ~1;
    break;
  case 1:
    dado2 = dado2 & ~2;
    break;
  case 2:
    dado2 = dado2 & ~4;
    break;
  case 3:
    dado2 = dado2 & ~8;
    break;
  case 4:
    dado2 = dado2 & ~16;
    break;
  case 5:
    dado2 = dado2 & ~32;
    break;
  case 6:
    dado2 = dado2 & ~64;
    break;
  case 7:
    dado2 = dado2 & ~128;
    break;
  }
}


// Purpose:    Turn a pixel on a graphic LCD on or off
// Inputs:     1) x - the x coordinate of the pixel
//             2) y - the y coordinate of the pixel
//             3) color - ON or OFF
void glcd_pixel(byte x, byte y, byte color)
{
   byte data;
   byte side = GLCD_LEFT;  // Stores which chip to use on the LCD

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   GLCD_DI=0;                                   // Set for instruction
   //bit_clear(x,7);                              // Clear the MSB. Part of an instruction code
   x = x & 0b01111111;
   //bit_set(x,6);                                // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
  data=0;
  
   if(color == ON)
     data = data | (1 << (y%8));
   else
      data = data & ~(1 << (y%8));
   GLCD_DI=1;                    // Set for data
   glcd_writeByte(side, data);   // Write the pixel data
}



void teste_linha(byte x, byte y)
{
   byte data;
   byte side = GLCD_LEFT;  // Stores which chip to use on the LCD

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   GLCD_DI=0;                                   // Set for instruction
   //bit_clear(x,7);                              // Clear the MSB. Part of an instruction code
   x = x & 0b01111111;
   //bit_set(x,6);                                // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
    
   data = 3;
   

   GLCD_DI=0;                    // Set for instruction
   glcd_writeByte(side, x);      // Set the horizontal address
   GLCD_DI=1;                    // Set for data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
   glcd_writeByte(side, data);   // Write the pixel data
}

void teste_nome(byte x, byte y, byte num, const unsigned char nomes[])
{
   byte data;
   byte side = GLCD_LEFT;  // Stores which chip to use on the LCD
   unsigned int j=0;

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   GLCD_DI=0;                                  // Set for instruction
   //bit_clear(x,7);                              // Clear the MSB. Part of an instruction code
   x = x & 0b01111111;
   //bit_set(x,6);                                // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
   //GLCD_DI=1;                                   // Set for data
   //glcd_readByte(side);                         // Need two reads to get data
   //data = glcd_readByte(side);                  //  at new addre
   

   GLCD_DI=0;                    // Set for instruction
   glcd_writeByte(side, x);      // Set the horizontal address
   GLCD_DI=1;                    // Set for data
   
   while(j !=num){
    data=nomes[j]; 
    glcd_writeByte(side, data);   // Write the pixel data
    j++;
   }

}

void delay(unsigned int tempo){
  
  unsigned char TEMP=0;
    for(;tempo;tempo--){
      for(TEMP=CPU_CLOCK/72;TEMP;TEMP--)
      {};
      __RESET_WATCHDOG();
    }
}


  // Cria um semaforo com contador = 1, informando que o recurso está disponível
  // após a inicialização
void init_glcd_resource(INT8U priority)
{  
  // Cria um mutex com contador = 1, informando que o recurso está disponível
  // após a inicialização
  // Prioridade máxima a acessar o recurso = priority
  if (OSMutexCreate(&GLCD,priority) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
}
