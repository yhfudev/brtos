#include "hardware.h"
#include "BRTOS.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"

extern BRTOS_Sem *SEMTESTE;


void Task_1(void)
{
    
  for (;;) 
  {
  	  // aguarda 2 semaforos... 
	  // assim, o atraso é 2 x 300 ticks (delay da task2)
	  OSSemPend(SEMTESTE,0);
	  OSSemPend(SEMTESTE,0);
      P1OUT ^=0x02;
      //DelayTask(300);
      
  }
}


void Task_2(void)
{
   /* task setup */
  
   /* task main loop */
   for (;;)
   {
      P1OUT ^=0x01;      
      DelayTask(300);
      OSSemPost(SEMTESTE);
      
   }
}

