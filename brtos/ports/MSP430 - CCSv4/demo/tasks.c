#include "BRTOS/includes/BRTOS.h"

extern BRTOS_Sem *SEMTESTE;
extern BRTOS_Queue *Serial;

void System_Time(void)
{   
   OSResetTime(&Hora);
   OSResetDate(&Data);
  
   INT16U i = 0;
   // task main loop
   for (;;)
   {      
      //DelayTask(configTICK_RATE_HZ);
      DelayTask(10);
      i++;
      if(i>=100){
	      OSUpdateUptime(&Hora,&Data);
	      i=0; 
	      /** LED toggle */
	      P1OUT ^=0x02;          
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
            acquireUART();
            Transmite_CPU_Load();
            releaseUART();
            break;
          #endif
          case '2':
            acquireUART();
            Transmite_Uptime();
            releaseUART();
            break;
          case '3':
            acquireUART();
            Serial_Envia_Frase((CHAR8*)version);
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);
            releaseUART();
            break;
          case '4':
           acquireUART();
            Transmite_RAM_Ocupada();
            releaseUART();
            break;
          case '5':
            acquireUART();
            Transmite_Task_Stacks();
            releaseUART();
            break;
          case '6':
            acquireUART();
            Reason_of_Reset();
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseUART();
            break;
          #if (OSTRACE == 1) 
          case '7':
            acquireUART();
            Send_OSTrace();
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseUART();
            break;            
          #endif
          default:
            acquireUART();
            Serial_Envia_Caracter(pedido);
            Serial_Envia_Caracter(LF);
            Serial_Envia_Caracter(CR);            
            releaseUART();
            break;
        }
      }
   }   
}


void Task_1(void)
{
  
  for (;;) 
  {
  	  /** aguarda 2 semaforos... 
	  assim, o atraso é 2 x 300 ticks (delay da task2)
	  */
	  /** wait 2 sema posts -  600 ticks */
	  OSSemPend(SEMTESTE,0);
	  OSSemPend(SEMTESTE,0);
  
  }
}


void Task_2(void)
{
   /** task setup */
  
   /** task main loop */
   for (;;)
   {
      /** LED toggle */
	  P1OUT ^=0x01;      
	  /** wait 300 ticks */
      DelayTask(20);
	  /** post sema */
      OSSemPost(SEMTESTE);
      
   }
}

