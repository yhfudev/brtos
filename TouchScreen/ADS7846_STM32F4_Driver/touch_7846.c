#include "BRTOS.h"
#include "touch_7846.h"
#include "SSD1289.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Pen_Holder Pen_Point;


unsigned char flag=0;

														
unsigned char SPI_WriteByte(u8 num)    
{  
  unsigned char Data = 0;
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)==RESET);
  SPI_I2S_SendData(SPI3,num);
  while(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_RXNE)==RESET);
  Data = SPI_I2S_ReceiveData(SPI3);

  return Data; 	 				   
} 	


static u16 tp_read_value(u8 cmd)
{
	u8  read = 0;
	u16 ret  = 0;

	T_CS();
	SPI_WriteByte(cmd);
	read = SPI_WriteByte(0);
    ret = (u16)(read << 5);
    read = SPI_WriteByte(0);
    ret = ret | (read >> 3);
    T_DCS();

	return ret;
}

static u16 tp_filter(u16 *sampleBuf)
{
	u16 temp;
	int i,j;

	for(i = 0; i < 4; i++)
	{
		for(j=i; j < 7; j++)
		{
			if(sampleBuf[i] > sampleBuf[j])
			{
				/* Swap the values */
				temp = sampleBuf[i];
				sampleBuf[i] = sampleBuf[j];
				sampleBuf[j] = temp;
			}
		}
	}
	return sampleBuf[3];
}

static u16 Read_TP_Axis(u8 axis)
{
	int i = 0;
	u16 buffer[7];

	// Valid values are 0 or 1
	if (axis > 1)
		return 0;

	if (axis == AXIS_X)
		axis = 0xD1;
	else
		axis = 0x91;

	/* Discard the first conversion - very noisy and keep the ADC on hereafter
	 * till we are done with the sampling. Note that PENIRQ is disabled.
 	 */
	(void)tp_read_value(axis);

	for(i = 0; i < 7; i++)
	{
		buffer[i]=tp_read_value(axis);
	}

	/* Switch on PENIRQ once again - perform a dummy read */
	(void)tp_read_value((u8)(axis - 1));

	return tp_filter(buffer);
}



void TP_Read(void)
{
   Pen_Point.X=Read_TP_Axis(AXIS_X);
   Pen_Point.Y=Read_TP_Axis(AXIS_Y);

   Pen_Point.X0=(int)((Pen_Point.X-Pen_Point.xoff)/Pen_Point.xfac);
   Pen_Point.Y0=(int)((Pen_Point.Y-Pen_Point.yoff)/Pen_Point.yfac);
   if(Pen_Point.X0>=240)
   {
     Pen_Point.X0=239;
   }
   if(Pen_Point.Y0>=320)
   {
     Pen_Point.X0=319;
   }
}

void calib_touch(void)
{
	u16 x1,x2,y1,y2;

	LCD_DrawLine(5, 5, 10, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(10, 0, 10, LCD_DIR_VERTICAL);

	while(1)
	{
	  // If the touch panel is pressed
	  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0)
	  {
		  // Read position
		  x1=Read_TP_Axis(AXIS_X);
		  y1=Read_TP_Axis(AXIS_Y);
		  break;
	  }
	  DelayTask(2);
	}

	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0);
	DelayTask(100);

	LCD_Clear(BLACK);

	LCD_DrawLine(115, 160, 10, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(120, 155, 10, LCD_DIR_VERTICAL);

	while(1)
	{
	  // If the touch panel is pressed
	  if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0)
	  {
		  // Read position
		  x2=Read_TP_Axis(AXIS_X);
		  y2=Read_TP_Axis(AXIS_Y);
		  break;
	  }
	  DelayTask(2);
	}

	while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0);
	DelayTask(300);

	Pen_Point.xfac = (float)((float)(x2 - x1) / (float)(120 - 10));
	Pen_Point.yfac = (float)((float)(y2 - y1) / (float)(160 - 5));

	Pen_Point.xoff = x1 - (short)(10 * Pen_Point.xfac);
	Pen_Point.yoff = y1 - (short)(5 * Pen_Point.yfac);

	LCD_Clear(BLACK);
}


void touch_init(void)
{	
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStruct;  
  //NVIC_InitTypeDef NVIC_InitStructure;
  //EXTI_InitTypeDef EXTI_InitStructure;
 
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_100MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  

  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_Init(GPIOC,&GPIO_InitStruct);
   	  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);      //sclk	10	 13
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);		//mýso	11	 14
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);		//mosý	12	 15
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
												   
  SPI_I2S_DeInit(SPI3);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//SPI_CPOL_Low 	 SPI_CPOL_High
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard	 //SPI_NSS_Soft
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; 	//16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStructure.SPI_CRCPolynomial = 7; 
  SPI_Init(SPI3,&SPI_InitStructure);
  SPI_Cmd(SPI3,ENABLE);
  //CS
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;  // 3
  GPIO_Init(GPIOC,&GPIO_InitStruct);    // d
  T_DCS(); 				 
  //T_PEN

  //T_PEN
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
  GPIO_Init(GPIOC,&GPIO_InitStruct);

  /*
  GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;  
  GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
  GPIO_Init(GPIOD,&GPIO_InitStruct);     
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);  	 
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  */
}



/*
void EXTI9_5_IRQHandler(void)		
{
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  { 
   	  
   EXTI_ClearITPendingBit(EXTI_Line6);
   // Read_Ads7846(); 
  }  
}
*/

void Drow_Touch_Point(uint16_t x,uint16_t y)
{
	LCD_DrawUniLine(x-12,y,x+13,y);
	LCD_DrawUniLine(x,y-12,x,y+13);
	Pixel(x+1,y+1,BLUE);
	Pixel(x-1,y+1,BLUE);
	Pixel(x+1,y-1,BLUE);
	Pixel(x-1,y-1,BLUE);
	LCD_DrawCircle(x,y,6);
}	  
void Draw_Big_Point(uint16_t x,uint16_t y)
{	    
	Pixel(x,y,BLUE);
	Pixel(x+1,y,BLUE);
	Pixel(x,y+1,BLUE);
	Pixel(x+1,y+1,BLUE);
}		
