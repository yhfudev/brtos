/**
* \file graphic.c
* \brief Graphic LCD drawing functions.
*
* This file contain a font of size 5x7 and the drawing functions of the graphic lcd, such as:
* Draw lines, rectangles, bars and circles. Thus, draw text with the specific font and the bitmap files.
*
**/

#include "hardware.h"
#include "glcd.h"
#include "graphic.h"
#include "event.h"
#include "BRTOS.h"
#include "drivers.h"
#include <string.h>
#include <stdlib.h>

#pragma warn_implicitconv off

/////////////////////////////////////////////////////////////////////////
#ifndef GRAPHICS_DRAWING_FUNCTIONS
#define GRAPHICS_DRAWING_FUNCTIONS
/////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////
//// Defines a 5x7 font
/////////////////////////////////////////////////////////////////////////
const byte FONT[96][5] =
                        {0x00, 0x00, 0x00, 0x00, 0x00, // SPACE
                         0x00, 0x00, 0x5F, 0x00, 0x00, // !
                         0x00, 0x03, 0x00, 0x03, 0x00, // "
                         0x14, 0x3E, 0x14, 0x3E, 0x14, // #
                         0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
                         0x43, 0x33, 0x08, 0x66, 0x61, // %
                         0x36, 0x49, 0x55, 0x22, 0x50, // &
                         0x00, 0x05, 0x03, 0x00, 0x00, // '
                         0x00, 0x1C, 0x22, 0x41, 0x00, // (
                         0x00, 0x41, 0x22, 0x1C, 0x00, // )
                         0x14, 0x08, 0x3E, 0x08, 0x14, // *
                         0x08, 0x08, 0x3E, 0x08, 0x08, // +
                         0x00, 0x50, 0x30, 0x00, 0x00, // ,
                         0x08, 0x08, 0x08, 0x08, 0x08, // -
                         0x00, 0x60, 0x60, 0x00, 0x00, // .
                         0x20, 0x10, 0x08, 0x04, 0x02, // /
                         0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
                         0x00, 0x04, 0x02, 0x7F, 0x00, // 1
                         0x42, 0x61, 0x51, 0x49, 0x46, // 2
                         0x22, 0x41, 0x49, 0x49, 0x36, // 3
                         0x18, 0x14, 0x12, 0x7F, 0x10, // 4
                         0x27, 0x45, 0x45, 0x45, 0x39, // 5
                         0x3E, 0x49, 0x49, 0x49, 0x32, // 6
                         0x01, 0x01, 0x71, 0x09, 0x07, // 7
                         0x36, 0x49, 0x49, 0x49, 0x36, // 8
                         0x26, 0x49, 0x49, 0x49, 0x3E, // 9
                         0x00, 0x36, 0x36, 0x00, 0x00, // :
                         0x00, 0x56, 0x36, 0x00, 0x00, // ;
                         0x08, 0x14, 0x22, 0x41, 0x00, // <
                         0x14, 0x14, 0x14, 0x14, 0x14, // =
                         0x00, 0x41, 0x22, 0x14, 0x08, // >
                         0x02, 0x01, 0x51, 0x09, 0x06, // ?
                         0x3E, 0x41, 0x59, 0x55, 0x5E, // @
                         0x7E, 0x09, 0x09, 0x09, 0x7E, // A
                         0x7F, 0x49, 0x49, 0x49, 0x36, // B
                         0x3E, 0x41, 0x41, 0x41, 0x22, // C
                         0x7F, 0x41, 0x41, 0x41, 0x3E, // D
                         0x7F, 0x49, 0x49, 0x49, 0x41, // E
                         0x7F, 0x09, 0x09, 0x09, 0x01, // F
                         0x3E, 0x41, 0x41, 0x49, 0x3A, // G
                         0x7F, 0x08, 0x08, 0x08, 0x7F, // H
                         0x00, 0x41, 0x7F, 0x41, 0x00, // I
                         0x30, 0x40, 0x40, 0x40, 0x3F, // J
                         0x7F, 0x08, 0x14, 0x22, 0x41, // K
                         0x7F, 0x40, 0x40, 0x40, 0x40, // L
                         0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
                         0x7F, 0x02, 0x04, 0x08, 0x7F, // N
                         0x3E, 0x41, 0x41, 0x41, 0x3E, // O
                         0x7F, 0x09, 0x09, 0x09, 0x06, // P
                         0x1E, 0x21, 0x21, 0x21, 0x5E, // Q
                         0x7F, 0x09, 0x09, 0x09, 0x76, // R
                         0x26, 0x49, 0x49, 0x49, 0x32, // S
                         0x01, 0x01, 0x7F, 0x01, 0x01, // T
                         0x3F, 0x40, 0x40, 0x40, 0x3F, // U
                         0x1F, 0x20, 0x40, 0x20, 0x1F, // V
                         0x7F, 0x20, 0x10, 0x20, 0x7F, // W
                         0x41, 0x22, 0x1C, 0x22, 0x41, // X
                         0x07, 0x08, 0x70, 0x08, 0x07, // Y
                         0x61, 0x51, 0x49, 0x45, 0x43, // Z
                         0x00, 0x7F, 0x41, 0x00, 0x00, // [
                         0x02, 0x04, 0x08, 0x10, 0x20, // \
                         0x00, 0x00, 0x41, 0x7F, 0x00, // ]
                         0x04, 0x02, 0x01, 0x02, 0x04, // ^
                         0x40, 0x40, 0x40, 0x40, 0x40, // _
                         0x00, 0x01, 0x02, 0x04, 0x00, // `
                         0x00, 0x01, 0x02, 0x04, 0x00, // `
                         0x20, 0x54, 0x54, 0x54, 0x78, // a
                         0x7F, 0x44, 0x44, 0x44, 0x38, // b
                         0x38, 0x44, 0x44, 0x44, 0x44, // c
                         0x38, 0x44, 0x44, 0x44, 0x7F, // d
                         0x38, 0x54, 0x54, 0x54, 0x18, // e
                         0x04, 0x04, 0x7E, 0x05, 0x05, // f
                         0x08, 0x54, 0x54, 0x54, 0x3C, // g
                         0x7F, 0x08, 0x04, 0x04, 0x78, // h
                         0x00, 0x44, 0x7D, 0x40, 0x00, // i
                         0x20, 0x40, 0x44, 0x3D, 0x00, // j
                         0x7F, 0x10, 0x28, 0x44, 0x00, // k
                         0x00, 0x41, 0x7F, 0x40, 0x00, // l
                         0x7C, 0x04, 0x78, 0x04, 0x78, // m
                         0x7C, 0x08, 0x04, 0x04, 0x78, // n
                         0x38, 0x44, 0x44, 0x44, 0x38, // o
                         0x7C, 0x14, 0x14, 0x14, 0x08, // p
                         0x08, 0x14, 0x14, 0x14, 0x7C, // q
                         0x00, 0x7C, 0x08, 0x04, 0x04, // r
                         0x48, 0x54, 0x54, 0x54, 0x20, // s
                         0x04, 0x04, 0x3F, 0x44, 0x44, // t
                         0x3C, 0x40, 0x40, 0x20, 0x7C, // u
                         0x1C, 0x20, 0x40, 0x20, 0x1C, // v
                         0x3C, 0x40, 0x30, 0x40, 0x3C, // w
                         0x44, 0x28, 0x10, 0x28, 0x44, // x
                         0x0C, 0x50, 0x50, 0x50, 0x3C, // y
                         0x44, 0x64, 0x54, 0x4C, 0x44, // z
                         0x00, 0x08, 0x36, 0x41, 0x41, // {
                         0x00, 0x00, 0x7F, 0x00, 0x00, // |
                         0x41, 0x41, 0x36, 0x08, 0x00, // }
                         0x02, 0x01, 0x02, 0x04, 0x02};// ~
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a line on a graphic LCD using Bresenham's
//                line drawing algorithm
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                color - ON or OFF
// Dependencies:  glcd_pixel()
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_line(int x1, int y1, int x2, int y2, byte color)
#else
void glcd_line(byte x1, byte y1, byte x2, byte y2, byte color)
#endif
{
   int        dy, dx;
   signed char  addx=1, addy=1;
   signed int P, diff;

   #ifdef LARGE_LCD
   int i=0;
   dx = abs((signed int)(x2 - x1));
   dy = abs((signed int)(y2 - y1));
   #else
   byte i=0;
   dx = abs((signed char)(x2 - x1));
   dy = abs((signed char)(y2 - y1));
   #endif

   if(x1 > x2)
      addx = -1;
   if(y1 > y2)
      addy = -1;

   if(dx >= dy)
   {
      dy *= 2;
      P = dy - dx;
      diff = P - dx;

      for(; i<=dx; ++i)
      {
         glcd_pixel(x1, y1, color);

         if(P < 0)
         {
            P  += dy;
            x1 += addx;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
   else
   {
      dx *= 2;
      P = dx - dy;
      diff = P - dy;

      for(; i<=dy; ++i)
      {
         glcd_pixel(x1, y1, color);

         if(P < 0)
         {
            P  += dx;
            y1 += addy;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a rectangle on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                fill  - YES or NO
//                color - ON or OFF
// Dependencies:  glcd_pixel(), glcd_line()
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_rect(int x1, int y1, int x2, int y2, byte fill, byte color)
#else
void glcd_rect(byte x1, byte y1, byte x2, byte y2, byte fill, byte color)
#endif
{
   if(fill)
   {
      #ifdef LARGE_LCD
      int i, xmin, xmax, ymin, ymax;
      #else
      byte  i, xmin, xmax, ymin, ymax;
      #endif

      if(x1 < x2)                            //  Find x min and max
      {
         xmin = x1;
         xmax = x2;
      }
      else
      {
         xmin = x2;
         xmax = x1;
      }

      if(y1 < y2)                            // Find the y min and max
      {
         ymin = y1;
         ymax = y2;
      }
      else
      {
         ymin = y2;
         ymax = y1;
      }

      for(; xmin <= xmax; ++xmin)
      {
         for(i=ymin; i<=ymax; ++i)
         {
            glcd_pixel(xmin, i, color);
         }
      }
   }
   else
   {
      glcd_line(x1, y1, x2, y1, color);      // Draw the 4 sides
      glcd_line(x1, y2, x2, y2, color);
      glcd_line(x1, y1, x1, y2, color);
      glcd_line(x2, y1, x2, y2, color);
   }
}

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a bar (wide line) on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                width  - The number of pixels wide
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_bar(int x1, int y1, int x2, int y2, byte width, byte color)
#else
void glcd_bar(byte x1, byte y1, byte x2, byte y2, byte width, byte color)
#endif
{
   signed char         half_width;
   signed int dy, dx;
   signed char  addx=1, addy=1, j;
   signed int P, diff, c1, c2;

   #ifdef LARGE_LCD
   int i=0;
   dx = abs((signed int)(x2 - x1));
   dy = abs((signed int)(y2 - y1));
   #else
   byte i=0;
   dx = abs((signed char)(x2 - x1));
   dy = abs((signed char)(y2 - y1));
   #endif

   half_width = width/2;
   c1 = -(dx*x1 + dy*y1);
   c2 = -(dx*x2 + dy*y2);

   if(x1 > x2)
   {
      signed int temp;
      temp = c1;
      c1 = c2;
      c2 = temp;
      addx = -1;
   }
   if(y1 > y2)
   {
      signed int temp;
      temp = c1;
      c1 = c2;
      c2 = temp;
      addy = -1;
   }

   if(dx >= dy)
   {
      P = 2*dy - dx;
      diff = P - dx;

      for(i=0; i<=dx; ++i)
      {
         for(j=-half_width; j<half_width+width%2; ++j)
         {
            #ifdef LARGE_LCD
            int temp;
            #else
            byte temp;
            #endif

            temp = dx*x1+dy*(y1+j);    // Use more RAM to increase speed
            if(temp+c1 >= 0 && temp+c2 <=0)
               glcd_pixel(x1, y1+j, color);
         }
         if(P < 0)
         {
            P  += 2*dy;
            x1 += addx;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
   else
   {
      P = 2*dx - dy;
      diff = P - dy;

      for(i=0; i<=dy; ++i)
      {
         if(P < 0)
         {
            P  += 2*dx;
            y1 += addy;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
         for(j=-half_width; j<half_width+width%2; ++j)
         {
            #ifdef LARGE_LCD
            int temp;
            #else
            byte temp;
            #endif

            temp = dx*x1+dy*(y1+j);    // Use more RAM to increase speed
            if(temp+c1 >= 0 && temp+c2 <=0)
               glcd_pixel(x1+j, y1, color);
         }
      }
   }
}


/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a circle on a graphic LCD
// Inputs:        (x,y) - the center of the circle
//                radius - the radius of the circle
//                fill - YES or NO
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_circle(int x, int y, int radius, byte fill, byte color)
#else
void glcd_circle(byte x, byte y, byte radius, byte fill, byte color)
#endif
{
   #ifdef LARGE_LCD
   signed int a, b, P;
   #else
   signed char  a, b, P;
   #endif

   a = 0;
   b = radius;
   P = 1 - radius;

   do
   {
      if(fill)
      {
         glcd_line(x-a, y+b, x+a, y+b, color);
         glcd_line(x-a, y-b, x+a, y-b, color);
         glcd_line(x-b, y+a, x+b, y+a, color);
         glcd_line(x-b, y-a, x+b, y-a, color);
      }
      else
      {
         glcd_pixel(a+x, b+y, color);
         glcd_pixel(b+x, a+y, color);
         glcd_pixel(x-a, b+y, color);
         glcd_pixel(x-b, a+y, color);
         glcd_pixel(b+x, y-a, color);
         glcd_pixel(a+x, y-b, color);
         glcd_pixel(x-a, y-b, color);
         glcd_pixel(x-b, y-a, color);
      }

      if(P < 0)
         P += 3 + 2 * a++;
      else
         P += 5 + 2 * (a++ - b--);
    } while(a <= b);
}


/////////////////////////////////////////////////////////////////////////
// Purpose:       Write text on a graphic LCD
// Inputs:        (x,y) - The upper left coordinate of the first letter
//                textptr - A pointer to an array of text to display
//                size - The size of the text: 1 = 5x7, 2 = 10x14, ...
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
void glcd_text57(byte x, byte y, char *string) {
   
   byte data=0;
   byte side = GLCD_LEFT;  // Stores which chip to use on the LCD
   unsigned char k=0;
   unsigned char z=0;
   byte x_pos=0;

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   GLCD_DI=0;
   
   x_pos = x;                                  // Set for instruction
 
   x = x & 0b01111111;
                                                 // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
   //GLCD_DI=1;                                   // Set for data
   //glcd_readByte(side);                         // Need two reads to get data
   //data = glcd_readByte(side);                  //  at new addre
   

   //GLCD_DI=0;                    // Set for instruction
   //glcd_writeByte(side, x);      // Set the horizontal address
   GLCD_DI=1;                    // Set for data
   
   k=0;
   while(*string){
    z=0;
    k=*string;
    k=k-32;
    while(z !=6){
      if (z !=5){
        data = FONT[k][z]; 
      }else{
        
        data = 0;
      }
      glcd_writeByte(side, data);   // Write the pixel data
      z++;
      x_pos++;
      if (side == GLCD_LEFT){
        if (x_pos > 63){
          x_pos = 0;
          x = 0;
          side = GLCD_RIGHT;
          x = x & 0b01111111;                                                 // Set bit 6. Also part of an instruction code
          x = x | 0b01000000;
          GLCD_DI=0;                    // Set for instruction
          glcd_writeByte(side, x);                     // Set the horizontal address
          delay(2);
          glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
          delay(2);
          GLCD_DI=1;                    // Set for data
        }
      }
    }
    k=0;
    string++;
   }
}

void write_string(INT8U x, INT8U y, INT16U size, INT8U string[])
{
   byte data=0;
   byte side = GLCD_LEFT;  // Stores which chip to use on the LCD
   unsigned char k=0;
   unsigned char z=0;
   byte x_pos=0;
   INT16U index=0;   

   if(x > 63)              // Check for first or second display area
   {
      x -= 64;
      side = GLCD_RIGHT;
   }

   GLCD_DI=0;
   
   x_pos = x;                                  // Set for instruction
 
   x = x & 0b01111111;
                                                 // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                     // Set the horizontal address
   glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
   GLCD_DI=1;                               // Set for data
   
   k=0;
   
   while(index<size)
   {
    z=0;
    k = string[index];
    
      k=k-32;
      while(z !=6){
        if (z !=5){
          data = FONT[k][z]; 
        }else{
        
          data = 0;
        }
        glcd_writeByte(side, data);   // Write the pixel data
        z++;
        x_pos++;
        if (side == GLCD_LEFT)
        {
          if (x_pos > 63){
            x_pos = 0;
            x = 0;
            side = GLCD_RIGHT;
            x = x & 0b01111111;                                                 // Set bit 6. Also part of an instruction code
            x = x | 0b01000000;
            GLCD_DI=0;                    // Set for instruction
            glcd_writeByte(side, x);                     // Set the horizontal address
            delay(2);
            glcd_writeByte(side, (y/8 & 0xBF) | 0xB8);   // Set the vertical page address
            delay(2);
            GLCD_DI=1;                    // Set for data
          }
        }
    }
    k=0;
    index++;
   }   
}



void bitmap(INT8U x, INT8U y, INT16U size, const INT8U figure[])
{
   INT8U data=0;
   INT8U side = GLCD_LEFT;  // Stores which chip to use on the LCD
   INT8U x_pos=0;
   INT8U y_pos=0;
   INT16U index=0;

   GLCD_DI=0;
   
   x_pos = x;                                     // Set for instruction
   y_pos = y;
 
   x = x & 0b01111111;
                                                  // Set bit 6. Also part of an instruction code
   x = x | 0b01000000;
   glcd_writeByte(side, x);                       // Set the horizontal address
   delay(2);   
   glcd_writeByte(side, (y_pos/8 & 0xBF) | 0xB8); // Set the vertical page address
   delay(2);
   GLCD_DI=1;                                     // Set for data
   
   while(index<size)
   {
      data = figure[index];
      glcd_writeByte(side, data);   // Write the pixel data
      x_pos++;
      
      if(x_pos<64)
      {
        side = GLCD_LEFT;
      }
      else
      {
        side = GLCD_RIGHT;
      }      

      if(x_pos==64)
      { 
        GLCD_DI=0;        
        glcd_writeByte(side, (y_pos/8 & 0xBF) | 0xB8);   // Set the vertical page address      
        delay(2);
        x = 0;
        x = x & 0b01111111;                              // Set bit 6. Also part of an instruction code
        x = x | 0b01000000;        
        glcd_writeByte(side, x);                         // Set the horizontal address
        delay(2);        
        GLCD_DI=1;                                       // Set for data            
      }
      
      if(x_pos==128)
      { 
        x_pos = 0;
        y_pos = y_pos + 8;
        side = GLCD_LEFT;
        GLCD_DI=0;        
        glcd_writeByte(side, (y_pos/8 & 0xBF) | 0xB8);   // Set the vertical page address      
        delay(2);
        x = 0;
        x = x & 0b01111111;                              // Set bit 6. Also part of an instruction code
        x = x | 0b01000000;        
        glcd_writeByte(side, x);                         // Set the horizontal address
        delay(2);        

        GLCD_DI=1;                                       // Set for data            
      }
      
    index++;
   }
}


#endif
