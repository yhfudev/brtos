
#include "derivative.h" /* include peripheral declarations */
#include "BRTOS.h"
#include "drivers.h"
#include "OS_RTC.h"
#include <hidef.h> /* for EnableInterrupts macro */
//#include <stdlib.h>
//#include "event.h"
//#include "queue.h"
void Transmite_Uptime(void);
void Transmite_Duty_Cycle(void);
void Transmite_RAM_Ocupada(void);
void Transmite_Task_Stacks(void);


void Transmite_Uptime(void)
{
   INT8U caracter = 0;
   OSTime Uptime;
   OSDate UpDate;
   
   OSEnterCritical;
   Uptime = OSUptime();
   UpDate = OSUpDate();
   OSExitCritical;
   
   Serial_Envia_Frase("UPTIME: ");

   // Só funciona até 99 dias
   if(UpDate.RTC_Day > 0)
   {    
      if(UpDate.RTC_Day > 9)
      {
          caracter = (UpDate.RTC_Day / 10) + 48;
          Serial_Envia_Caracter(caracter);         
      }
    
      caracter = (UpDate.RTC_Day%10) + 48;
      Serial_Envia_Caracter(caracter);
      Serial_Envia_Caracter('d');
      Serial_Envia_Caracter(' ');      
   }
   
   if(Uptime.RTC_Hour > 9)
   {   
      caracter = (Uptime.RTC_Hour / 10) + 48;
      Serial_Envia_Caracter(caracter);
   }
   caracter = (Uptime.RTC_Hour%10) + 48;
   Serial_Envia_Caracter(caracter);
   Serial_Envia_Caracter('h');
   Serial_Envia_Caracter(' ');
   
   if(Uptime.RTC_Minute > 9)
   {   
      caracter = (Uptime.RTC_Minute / 10) + 48;
      Serial_Envia_Caracter(caracter);
   }
   caracter = (Uptime.RTC_Minute%10) + 48;
   Serial_Envia_Caracter(caracter);
   Serial_Envia_Caracter('m');
   Serial_Envia_Caracter(' ');

   if(Uptime.RTC_Second > 9)
   {
      caracter = (Uptime.RTC_Second / 10) + 48;
      Serial_Envia_Caracter(caracter);
   }
   caracter = (Uptime.RTC_Second%10) + 48;
   Serial_Envia_Caracter(caracter);   
   Serial_Envia_Caracter('s');
   Serial_Envia_Caracter(' ');   
   Serial_Envia_Caracter(10);
   Serial_Envia_Caracter(13);
}


void Transmite_Duty_Cycle(void)
{
    INT16U percent = 0;
    INT8U caracter = 0;
   
    Serial_Envia_Frase("DUTY CYCLE: ");
    
    //OSEnterCritical;
    //percent = (TPM3C1V*100)/TPM3MOD;
    //OSExitCritical;    
    percent = 50;
    
    caracter = (percent / 10) + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (percent%10) + 48;
    Serial_Envia_Caracter(caracter);
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
}

void Transmite_RAM_Ocupada(void)
{
    INT8U caracter = 0;
    INT8U mil,cent,dez;
    INT16U SPAddress = 0;
   
    Serial_Envia_Frase("MEMORY: ");
    
    OSEnterCritical;
    SPAddress = iStackAddress;
    OSExitCritical;
    
    //SPAddress = SPAddress + 57 + NUMBER_OF_TASKS*12;
    
    mil = (SPAddress / 1000);
    caracter = mil + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    cent = ((SPAddress - mil*1000)/100);
    caracter = cent + 48;
    Serial_Envia_Caracter(caracter);
    dez = ((SPAddress - 1000*mil - cent*100)/10);
    caracter = dez + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (SPAddress%10) + 48;
    Serial_Envia_Caracter(caracter);
    Serial_Envia_Frase(" of ");
    mil = (HEAP_SIZE / 1000);
    caracter = mil + 48;
    Serial_Envia_Caracter(caracter);
    cent = ((HEAP_SIZE - mil*1000)/100);
    caracter = cent + 48;
    Serial_Envia_Caracter(caracter);
    dez = ((HEAP_SIZE - 1000*mil - cent*100)/10);
    caracter = dez + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (HEAP_SIZE%10) + 48;
    Serial_Envia_Caracter(caracter);    
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
}


void Transmite_Task_Stacks(void)
{
    INT8U caracter = 0;
    INT8U mil,cent,dez;
    INT16U VirtualStack = 0;
    INT8U j = 0;
   
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
   
    Serial_Envia_Frase("TASK STACK:");
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
    
    for (j=1;j<=NumberOfInstalledTasks;j++)
    {  
      Serial_Envia_Frase("TASK ");
      Serial_Envia_Caracter(j+48);
      Serial_Envia_Frase(": ");
      
      OSEnterCritical;
      VirtualStack = ContextTask[j].StackInit - ContextTask[j].StackPoint;
      OSExitCritical;
      
      mil = (VirtualStack / 1000);
      caracter = mil + 48;
      if(caracter != 48)
        Serial_Envia_Caracter(caracter);
      cent = ((VirtualStack - mil*1000)/100);
      caracter = cent + 48;
      if(caracter != 48)
        Serial_Envia_Caracter(caracter);
      dez = ((VirtualStack - 1000*mil - cent*100)/10);
      caracter = dez + 48;
      Serial_Envia_Caracter(caracter);
      caracter = (VirtualStack%10) + 48;
      Serial_Envia_Caracter(caracter);    
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);
    }
    
    Serial_Envia_Frase("IDLE TASK: ");
    
    OSEnterCritical;  
    VirtualStack = ContextTask[NumberOfInstalledTasks+1].StackInit - ContextTask[NumberOfInstalledTasks+1].StackPoint;
    OSExitCritical;
      
    mil = (VirtualStack / 1000);
    caracter = mil + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    cent = ((VirtualStack - mil*1000)/100);
    caracter = cent + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    dez = ((VirtualStack - 1000*mil - cent*100)/10);
    caracter = dez + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (VirtualStack%10) + 48;
    Serial_Envia_Caracter(caracter);    
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);        
}


void Transmite_IRQ(void)
{
    INT8U caracter = 0;
    INT8U mil,cent,dez;
    extern INT16U iIRQ;
   
    Serial_Envia_Frase("IRQ ENTRIES: ");
    mil = (iIRQ / 1000);
    caracter = mil + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    cent = ((iIRQ - mil*1000)/100);
    caracter = cent + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    dez = ((iIRQ - 1000*mil - cent*100)/10);
    caracter = dez + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    caracter = (iIRQ%10) + 48;
    Serial_Envia_Caracter(caracter);   
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
}

void Transmite_CPU_Load(void)
{
    INT16U percent = 0;
    INT8U caracter = 0;
   
    OSEnterCritical;
    percent = OSDuty;
    percent = (percent*100)/TPM1MOD;
    OSExitCritical;   
   
    Serial_Envia_Frase("CPU LOAD: ");
    
    if (percent == 100)
    {
      Serial_Envia_Frase("100");
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);      
    }
    else
    {
      caracter = (percent / 10) + 48;
      Serial_Envia_Caracter(caracter);
      caracter = (percent%10) + 48;
      Serial_Envia_Caracter(caracter);
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);
    }
}


void Transmite_Uptime_Display(void)
{
   INT8U caracter = 0;
   OSTime Uptime;
   OSDate UpDate;
   INT8U cont = 0;
   INT8U i = 0;
   
   OSEnterCritical;
   Uptime = OSUptime();
   UpDate = OSUpDate();
   OSExitCritical;

   cont = 0;

   // Só funciona até 99 dias
   if(UpDate.RTC_Day > 0)
   {    
      if(UpDate.RTC_Day > 9)
      {
          caracter = (UpDate.RTC_Day / 10) + 48;
          caractere(caracter);
          cont++;         
      }
    
      caracter = (UpDate.RTC_Day%10) + 48;
      caractere(caracter);
      caractere('d');
      caractere(' ');
      cont++;
      cont++;
      cont++;      
   }

   
   if(Uptime.RTC_Hour > 9)
   {   
      caracter = (Uptime.RTC_Hour / 10) + 48;
      caractere(caracter);
      cont++;
   }
   caracter = (Uptime.RTC_Hour%10) + 48;
   caractere(caracter);
   caractere('h');
   caractere(' ');
   cont++;
   cont++;
   cont++;
   
   if(Uptime.RTC_Minute > 9)
   {   
      caracter = (Uptime.RTC_Minute / 10) + 48;
      caractere(caracter);
      cont++;
   }
   caracter = (Uptime.RTC_Minute%10) + 48;
   caractere(caracter);
   caractere('m');
   caractere(' ');
   cont++;
   cont++;
   cont++;

   if(Uptime.RTC_Second > 9)
   {
      caracter = (Uptime.RTC_Second / 10) + 48;
      caractere(caracter);
      cont++;
   }
   caracter = (Uptime.RTC_Second%10) + 48;
   caractere(caracter);   
   caractere('s');
   caractere(' ');
   cont++;
   cont++;
   cont++;
   
   cont = 16 - cont;
   
   for(i=0;i<cont;i++)
   {
      caractere(' ');
   }
}
