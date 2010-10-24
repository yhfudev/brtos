/**
* \file graphic.h
* \brief Graphic LCD defines and draw functions prototypes.
*
*
**/

#include "hardware.h"
#include "glcd.h"
#include <string.h>
#include <stdlib.h>
#include "event.h"
#include "BRTOS.h"


////////////////////////////////////////////////
////////////////////////////////////////////////
///  Definições do Módulo do Display Gráfico  //
////////////////////////////////////////////////
////////////////////////////////////////////////
#define saida        PTFD         // Porta dos dados
#define dir_saida    PTFDD        // Direcao da porta de dados
#define GLCD_CS1     PTGD_PTGD1   // Chip Selection 1
#define GLCD_CS2     PTHD_PTHD0   // Chip Selection 2
#define GLCD_DI      PTED_PTED4   // Data or Instruction input
#define GLCD_RW      PTED_PTED5   // Read/Write
#define GLCD_E       PTED_PTED6   // Enable
#define GLCD_RST     PTHD_PTHD1   // Reset
#define GLCD_WIDTH   128
#define GLCD_LEFT    0
#define GLCD_RIGHT   1
#define CPU_CLOCK    24008000

#ifndef ON
#define ON  1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef YES
#define YES 1
#endif

#ifndef NO
#define NO  0
#endif

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a line on a graphic LCD using Bresenham's
//                line drawing algorithm
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                color - ON or OFF
// Dependencies:  glcd_pixel()
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_line(int x1, int y1, int x2, int y2, byte color);
#else
void glcd_line(byte x1, byte y1, byte x2, byte y2, byte color);
#endif

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a rectangle on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                fill  - YES or NO
//                color - ON or OFF
// Dependencies:  glcd_pixel(), glcd_line()
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_rect(int x1, int y1, int x2, int y2, byte fill, byte color);
#else
void glcd_rect(byte x1, byte y1, byte x2, byte y2, byte fill, byte color);
#endif

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a bar (wide line) on a graphic LCD
// Inputs:        (x1, y1) - the start coordinate
//                (x2, y2) - the end coordinate
//                width  - The number of pixels wide
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_bar(int x1, int y1, int x2, int y2, byte width, byte color);
#else
void glcd_bar(byte x1, byte y1, byte x2, byte y2, byte width, byte color);
#endif

/////////////////////////////////////////////////////////////////////////
// Purpose:       Draw a circle on a graphic LCD
// Inputs:        (x,y) - the center of the circle
//                radius - the radius of the circle
//                fill - YES or NO
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
#ifdef LARGE_LCD
void glcd_circle(int x, int y, int radius, byte fill, byte color);
#else
void glcd_circle(byte x, byte y, byte radius, byte fill, byte color);
#endif


/////////////////////////////////////////////////////////////////////////
// Purpose:       Write text on a graphic LCD
// Inputs:        (x,y) - The upper left coordinate of the first letter
//                textptr - A pointer to an array of text to display
//                size - The size of the text: 1 = 5x7, 2 = 10x14, ...
//                color - ON or OFF
/////////////////////////////////////////////////////////////////////////
void glcd_text57(byte x, byte y, char *text);

