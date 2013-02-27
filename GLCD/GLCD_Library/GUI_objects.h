/**
* \file GUI_objects.h
* \brief BRTOS LCD Graphical Objects header file
*
* Author: Miguel Moreto
*
*
**/

#include "gdisp.h"

/* Library configuration */

void GUI_ObjetcsInit(void);

#define USE_ROUNDED_BOX TRUE

#if (USE_ROUNDED_BOX == TRUE)
/* Structure typedef to store RoundBox parameters */
typedef struct _RoundBox
{
	coord_t x; // Upper left corner x screen position
	coord_t y; // Upper left corner y screen position
	coord_t dx; // x size (width)
	coord_t dy; // y size (height)
	coord_t radius; // radius of the corners (in pixels)
	color_t bg_color; // Background color of the box.
	color_t font_color; // Text color.
	char *str; // Pointer to a string.
}RoundBox_typedef;

/*Pointer to functions typedef */
typedef void (*Function)(RoundBox_typedef *a);
typedef void (*FuncInit)(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		RoundBox_typedef *RoundBox_struct);

/* Definitions of the functions associated with RoundBox GUI object */
typedef struct _FuncList
{
	Function Draw;
	Function Update;
	FuncInit Init;
}RoundBoxFunc_typedef;

extern RoundBoxFunc_typedef RoundBox;

/*
 * Declarations of the RoundBox functions.
 * ******************************************************************** */
void RoundBox_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		RoundBox_typedef *RoundBox_struct);

void RoundBox_Draw(RoundBox_typedef *RoundBox_struct);
#endif
