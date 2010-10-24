
/*********************************************************************************************************
*                                               BRTOS
*                                Brazilian Real-Time Operating System
*                            Acronymous of Basic Real-Time Operating System
*
*                              
*                                  Open Source RTOS under MIT License
*
*
*
*                                              OS Tasks
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/

#include "hardware.h"

#include "BRTOS.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"
#include "graphic.h"

#pragma warn_implicitconv off



// Declara um ponteiro para o bloco de controle da Porta Serial
extern BRTOS_Queue *Serial;
extern BRTOS_Sem   *SemTeste;


extern INT8U debug_display;
extern INT8U gedre_10_anos[1024];
extern INT8U brtos_logo[1024];





void System_Time(void)
{
   // task setup
   INT16U i = 0;
   
   OSResetTime(&Hora);
  
   // task main loop
   for (;;)
   {
      #if (WATCHDOG == 1)
        __RESET_WATCHDOG();
      #endif
      
      DelayTask(10);
      i++;
      
      if (i >= 100)
      {
        OSUpdateUptime(&Hora,&Data);
        i = 0;
      }
   }
}





void Task_Serial(void)
{
   /* task setup */
   INT8U pedido = 0;
   
   // task main loop
   for (;;) 
   {
      
      if(!OSQueuePend(Serial, &pedido, 0))
      {
        switch(pedido)
        {
          #if (COMPUTES_CPU_LOAD == 1)
          case '1':
            acquireSCI2();
            Transmite_CPU_Load();
            releaseSCI2();
            break;
          #endif
          case '2':
            acquireSCI2();
            Transmite_Uptime();
            releaseSCI2();
            break;
          case '3':
            acquireSCI2();
            Serial_Envia_Frase(version);
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);
            releaseSCI2();
            break;
          case '4':
            acquireSCI2();
            Transmite_RAM_Ocupada();
            releaseSCI2();
            break;
          case '5':
            acquireSCI2();
            Transmite_Task_Stacks();
            releaseSCI2();
            break;
          case '6':
            acquireSCI2();
            Reason_of_Reset();
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseSCI2();
            break;
          #if (OSTRACE == 1) 
          case '7':
            acquireSCI2();
            Send_OSTrace();
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseSCI2();
            break;            
          #endif
          default:
            acquireSCI2();
            Serial_Envia_Caracter(pedido);
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseSCI2();
            break;
        }
      }
   }   
}


void Test_Task_1(void)
{
   /* task setup */
   INT8U cont = 0;
  
   /* task main loop */
   for (;;)
   {
      PTAD_PTAD0 = ~PTAD_PTAD0;
      cont++;
      DelayTask(20);
      
      //Acorda a tarefa Test_Task_2
      (void)OSSemPost(SemTeste);
   }
}





void Test_Task_2(void)
{
   /* task setup */
   PTAD = 0;
   PTADD = 3;
  
   /* task main loop */
   for (;;)
   {
      PTAD_PTAD1 = ~PTAD_PTAD1;
      (void)OSSemPend(SemTeste,15);
   }
}



void Task_5(void)
{
   /* task setup */
   INT8U sentido = 0;
   INT8U k = 0;        

   acquireGLCD();
   bitmap(0, 0, 1024, &brtos_logo);
   releaseGLCD();
   DelayTask(3000);
   acquireGLCD();
   bitmap(0, 0, 1024, &gedre_10_anos);  
   releaseGLCD();
   
   
   DelayTask(3000);
   glcd_fillScreen(OFF);                // Clear the display   
   
   for (;;) 
   {
        
      if (sentido == 0)
      {
         if (k == 56)
            sentido = 1;
         acquireGLCD();
         glcd_fillScreen(OFF);                // Clear the display
         glcd_text57(0,k," BRTOS 1.0 ");
         releaseGLCD();
         if (k != 56)
          k = (INT8U)(k + 8);
      }
      else
      {
         if (k == 0)
            sentido = 0;        
         acquireGLCD();
         glcd_fillScreen(OFF);                // Clear the display
         glcd_text57(64,k," BRTOS 1.0 ");
         releaseGLCD();
         if (k)
          k = (INT8U)(k - 8);
      }
        
      DelayTask(800);
   }
}



// GPS Network Associate Timeout Function
void BRTOS_TimerHook(void)
{


}


