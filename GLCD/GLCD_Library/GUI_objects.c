/**
* \file GUI_objects.c
* \brief BRTOS LCD Graphical Objects
*
* Author: Miguel Moreto
*
*
**/

#include "GUI_objects.h"

/* Declaring RoundBox functions typedef */
RoundBoxFunc_typedef RoundBox;

/*  Initialization of the functions structures */
void GUI_ObjetcsInit(void)
{
#if (USE_ROUNDED_BOX == TRUE)
	RoundBox.Draw = RoundBox_Draw;
	RoundBox.Init = RoundBox_Init;
#endif
}

#if (USE_ROUNDED_BOX == TRUE)
/* Initializes the RoundBox structure */
void RoundBox_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		RoundBox_typedef *RoundBox_struct)
{
	RoundBox_struct->x = x;
	RoundBox_struct->y = y;
	RoundBox_struct->dx = width;
	RoundBox_struct->dy = height;
	RoundBox_struct->radius = radius;
	RoundBox_struct->bg_color = bg_color;
	RoundBox_struct->font_color = font_color;
	RoundBox_struct->str = str;
}

/* Function to draw a box with rounded corners */
void RoundBox_Draw(RoundBox_typedef *RoundBox_struct)
{
	coord_t x1, x2, y1, y2;
	coord_t size_x, size_y;

	x1 = RoundBox_struct->x + RoundBox_struct->radius;
	x2 = RoundBox_struct->x + RoundBox_struct->dx - RoundBox_struct->radius;
	y1 = RoundBox_struct->y + RoundBox_struct->radius;
	y2 = RoundBox_struct->y + RoundBox_struct->dy - RoundBox_struct->radius;

	// Draw the corners
	gdispFillCircle(x1,y1,RoundBox_struct->radius,RoundBox_struct->bg_color);
	gdispFillCircle(x2,y1,RoundBox_struct->radius,RoundBox_struct->bg_color);
	gdispFillCircle(x1,y2,RoundBox_struct->radius,RoundBox_struct->bg_color);
	gdispFillCircle(x2,y2,RoundBox_struct->radius,RoundBox_struct->bg_color);

	// Draw the inner rectangles.
	gdispFillArea(RoundBox_struct->x,y1,RoundBox_struct->radius,RoundBox_struct->dy-2*RoundBox_struct->radius,RoundBox_struct->bg_color);
	gdispFillArea(x2,y1,RoundBox_struct->radius+1,RoundBox_struct->dy-2*RoundBox_struct->radius,RoundBox_struct->bg_color);
	gdispFillArea(x1,RoundBox_struct->y,RoundBox_struct->dx-2*RoundBox_struct->radius,RoundBox_struct->dy+1,RoundBox_struct->bg_color);
	// Obtaining the size of the text.
	size_x = gdispGetStringWidth(RoundBox_struct->str, &fontLarger);
	size_y = gdispGetFontMetric(&fontLarger, fontHeight) -
			gdispGetFontMetric(&fontLarger, fontDescendersHeight);

	// Draw the string in the middle of the box.
	gdispDrawString(RoundBox_struct->x+(RoundBox_struct->dx-size_x)/2,
			RoundBox_struct->y+(RoundBox_struct->dy-size_y)/2,
			RoundBox_struct->str, &fontLarger, RoundBox_struct->font_color);

}
#endif
