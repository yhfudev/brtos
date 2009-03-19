#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include "BRTOS.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"



static void Task_1(void)
{
  /* task setup */
  OSResetTime(&Hora);
  /* task main loop */
  
  for (;;) 
  {
      OSUpdateUptime(&Hora,&Data);
      DelayTask(1000);
  }
}


static void Task_2(void)
{
   /* task setup */
   
  
   /* task main loop */
   for (;;)
   {
      #ifdef COP
        __RESET_WATCHDOG();
      #endif
      DelayTask(10);
   }
}

static void Task_3(void)
{
   /* task setup */
   
  
   /* task main loop */
   for (;;)
   {
      PTAD_PTAD0 = ~PTAD_PTAD0;
      DelayTask(20);
   }
}

static void Task_4(void)
{
   /* task setup */
   
  
   /* task main loop */
   for (;;)
   {
      PTAD_PTAD1 = ~PTAD_PTAD1;
      DelayTask(20);
   }
}
