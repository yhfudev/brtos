/**
* \file GUI_objects.h
* \brief BRTOS LCD Graphical Objects header file
*
* Author: Miguel Moreto
*
*
**/
#ifndef GUI_OBJECTS_H
#define GUI_OBJECTS_H

#include "gdisp.h"

/* Library configuration */
#define USE_BUTTON		TRUE
#define USE_SLIDER		TRUE
#define USE_CHECKBOX	TRUE
#define USE_GRAPH		TRUE


/* Object types enum */
enum
{
    NULL_OBJECT,
    BUTTON_OBJECT,
    CHECKBOX_OBJECT,
    SLIDER_OBJECT,
    GRAPH_OBJECT
} GUI_Objects;


/* Object event structure */
struct _ObjectEvent
{
	coord_t x1; 					// upper left corner x screen position
	coord_t y1; 					// upper left corner y screen position
	coord_t x2; 					// bottom right corner x screen position
	coord_t y2; 					// bottom left corner y screen position
	int		object;					// object type
	void	*ObjectPointer;
	struct _ObjectEvent *Next;
	struct _ObjectEvent *Previous;
};

typedef struct _ObjectEvent ObjectEvent_typedef;

/* Touch info structure */
typedef struct _TouchInfo
{
	coord_t x; // x screen position
	coord_t y; // y screen position
}Touch_typedef;

/* Objects function callbacks types */
typedef void (*Callbacks)(void);

/* Prototypes of the event handler external variables */
extern BRTOS_Queue	*TouchEvents;
extern BRTOS_Sem 	*TouchSync;

/* Declare background color */
extern color_t GuiBackground;


/* GUI event Handler */
#define GUI_Start_Handler()																		\
				while(1)																		\
				{																				\
	  	  	  	  	  /* If the touch panel is pressed */										\
	  	  	  	  	  if (OSDQueuePend(TouchEvents, (void*)&callback, 0) == READ_BUFFER_OK)		\
	  	  	  	  	  {																			\
	  	  	  	  		  /* Execute callback */												\
	  	  	  	  		  if (callback != NULL)													\
	  	  	  	  		  {																		\
	  	  	  	  			  callback();														\
	  	  	  	  		  }																		\
	  	  	  	  	  }																			\
	  	  	  	  }


/* GUI functions prototypes */
void GUI_ObjetcsInit(color_t background);
void GUI_IncludeObjectIntoEventList(ObjectEvent_typedef *object);
void GUI_RemoveObjectIntoEventList(ObjectEvent_typedef *object);
ObjectEvent_typedef *GUI_VerifyObjects(int x, int y);
void GUI_Event_Handler(void *param);


#if (USE_BUTTON == TRUE)
/* Structure typedef to store Button parameters */
typedef struct _Button
{
	coord_t x; 						// Upper left corner x screen position
	coord_t y; 						// Upper left corner y screen position
	coord_t dx; 					// x size (width)
	coord_t dy; 					// y size (height)
	coord_t radius; 				// radius of the corners (in pixels)
	color_t bg_color; 				// Background color of the box.
	color_t font_color; 			// Text color.
	char *str; 						// Pointer to a string.
	ObjectEvent_typedef event;		// Event handler
	Callbacks			ClickEvent;	// Clicked callback
}Button_typedef;

/*Pointer to button functions typedef */
typedef void (*Function)(Button_typedef *a);
typedef void (*FuncUpdate)(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		Button_typedef *Button_struct);
typedef void (*FuncInit1)(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		Button_typedef *Button_struct, Callbacks callback1);

/* Definitions of the functions associated with Button GUI object */
typedef struct _FuncList
{
	FuncInit1 	  Init;
	FuncUpdate 	  Update;
	Function 	  Draw;
	Function 	  Click;
}ButtonFunc_typedef;

extern ButtonFunc_typedef Button;

/*
 * Declarations of the Button functions.
 * ******************************************************************** */
void Button_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		Button_typedef *Button_struct, Callbacks click_event);

void Button_Update(coord_t x, coord_t y, coord_t width, coord_t height,
		coord_t radius, color_t bg_color, color_t font_color, char *str,
		Button_typedef *Button_struct);

void Button_Draw(Button_typedef *Button_struct);
void Button_Click(Button_typedef *Button_struct);
#endif


#if (USE_SLIDER == TRUE)
/* Structure typedef to store Slider parameters */
typedef struct _Slider
{
	coord_t x; 						// Upper left corner x screen position
	coord_t y; 						// Upper left corner y screen position
	coord_t dx; 					// x size (width)
	coord_t dy; 					// y size (height)
	coord_t x1; 					// Upper left corner x screen position - slider bar
	coord_t y1; 					// Upper left corner y screen position - slider bar
	coord_t x2; 					// x size (width) - slider bar
	coord_t y2; 					// y size (height) - slider bar
	coord_t radius; 				// radius of the corners (in pixels)
	color_t border_color; 			// Border color of the box.
	color_t fg_color; 				// Foreground color.
	int 	value;					// Slider value position
	int 	lvalue;					// Slider last value position
	ObjectEvent_typedef event;		// Event handler
	Callbacks			ClickEvent;	// Clicked callback
}Slider_typedef;

/*Pointer to Slider functions typedef */
typedef void (*FunctionS)(Slider_typedef *a);
typedef void (*FuncUpdateS)(int value, Slider_typedef *Slider_struct);
typedef void (*FuncInit1S)(coord_t x, coord_t y, coord_t width, coord_t height,
		color_t border_color, color_t fg_color, int value,
		Slider_typedef *Slider_struct, Callbacks callback1);

/* Definitions of the functions associated with Slider GUI object */
typedef struct _SliderFuncList
{
	FuncInit1S 	  Init;
	FuncUpdateS   Update;
	FunctionS 	  Draw;
	FunctionS 	  Click;
}SliderFunc_typedef;

extern SliderFunc_typedef Slider;

/*
 * Declarations of the Button functions.
 * ******************************************************************** */
void Slider_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		color_t bg_color, color_t fg_color, int value,
		Slider_typedef *Slider_struct, Callbacks click_event);

void Slider_Update(int value, Slider_typedef *Slider_struct);

void Slider_Draw(Slider_typedef *Slider_struct);
void Slider_Click(Slider_typedef *Slider_struct);
#endif


#if (USE_CHECKBOX == TRUE)
/* Structure typedef to store Checkbox parameters */
typedef struct _Checkbox
{
	coord_t x; 						// Upper left corner x screen position
	coord_t y; 						// Upper left corner y screen position
	coord_t dx; 					// x size (width)
	coord_t dy; 					// y size (height)
	coord_t radius; 				// radius of the corners (in pixels)
	color_t border_color; 			// Border color of the box.
	color_t fg_color; 				// Foreground color.
	unsigned char value;			// Slider value position
	ObjectEvent_typedef event;		// Event handler
	Callbacks			ClickEvent;	// Clicked callback
}Checkbox_typedef;

/*Pointer to Checkbox functions typedef */
typedef void (*FunctionC)(Checkbox_typedef *a);
typedef void (*FuncUpdateC)(int value, Checkbox_typedef *Checkbox_struct);
typedef void (*FuncInit1C)(coord_t x, coord_t y, coord_t width, coord_t height,
		color_t border_color, color_t fg_color, unsigned char value,
		Checkbox_typedef *Checkbox_struct, Callbacks callback1);

/* Definitions of the functions associated with Slider GUI object */
typedef struct _CheckboxFuncList
{
	FuncInit1C 	  Init;
	FuncUpdateC   Update;
	FunctionC 	  Draw;
	FunctionC 	  Click;
}CheckboxFunc_typedef;

extern CheckboxFunc_typedef Checkbox;

/* Initializes the Checkbox structure */
void Checkbox_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		color_t border_color, color_t fg_color, unsigned char value,
		Checkbox_typedef *Checkbox_struct, Callbacks click_event);

void Checkbox_Update(int value, Checkbox_typedef *Checkbox_struct);
void Checkbox_Draw(Checkbox_typedef *Checkbox_struct);
void Checkbox_Click(Checkbox_typedef *Checkbox_struct);

#endif


#if (USE_GRAPH == TRUE)

typedef struct _Trace
{
	int 	line_type;
	int		line_thin;
	color_t color; 					// Trace color.
}Trace_typedef;

/* Structure typedef to store Slider parameters */
typedef struct _Graph
{
	coord_t x; 						// Upper left corner x screen position
	coord_t y; 						// Upper left corner y screen position
	coord_t dx; 					// x size (width)
	coord_t dy; 					// y size (height)
	coord_t x1; 					// Upper left corner x screen position - slider bar
	coord_t y1; 					// Upper left corner y screen position - slider bar
	coord_t x2; 					// x size (width) - slider bar
	coord_t y2; 					// y size (height) - slider bar
	coord_t axis;					// Graph x axis
	coord_t radius; 				// radius of the corners (in pixels)
	color_t border_color; 			// Border color of the box.
	color_t fg_color; 				// Foreground color.
	int					ntraces;	// Number of traces
	Trace_typedef 		*traces;	// Graph traces
	char		  		*title_str;	// Title text
	char		  		*axisx_str;	// X axis text
	char				*axisy_str;	// Y axis text
	ObjectEvent_typedef event;		// Event handler
	Callbacks			ClickEvent;	// Clicked callback
}Graph_typedef;



void Graph_Init(coord_t x, coord_t y, coord_t width, coord_t height,
		color_t border_color, color_t fg_color, Trace_typedef *traces, int ntraces,
		char *title, char *axisx, char *axisy,
		Graph_typedef *Graph_struct, Callbacks click_event);

/* Initializes the Button structure */
void Graph_AddTraceData(Graph_typedef *Graph_struct, int *data);

/* Function to draw a box with rounded corners */
void Graph_Draw(Graph_typedef *Graph_struct);

#endif


#endif

