
#include "hardware.h"
#include "BRTOS.h"
#include "drivers.h"
#include "OS_RTC.h"

#pragma warn_implicitconv off




void Transmite_Uptime(void);
void Transmite_Duty_Cycle(void);
void Transmite_RAM_Ocupada(void);
void Transmite_Task_Stacks(void);


void Transmite_Uptime(void)
{
   INT8U caracter = 0;
   OSTime Uptime;
   OSDate UpDate;
   
   UserEnterCritical();
   Uptime = OSUptime();
   UpDate = OSUpDate();
   UserExitCritical();
   
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
   
    Serial_Envia_Frase("DUTY CYCLE TPM2CH0: ");
    
    UserEnterCritical();
    percent = (TPM2C0V*100)/TPM2MOD;
    UserExitCritical();    
    
    caracter = (percent / 10) + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (percent%10) + 48;
    Serial_Envia_Caracter(caracter);
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
}


void Transmite_Dimming_Level(INT8U level)
{
    INT8U cent = 0;
    INT8U dez = 0;
    INT8U caracter = 0;
   
    Serial_Envia_Frase("DUTY CYCLE: ");
    
    cent = (level/100);
    caracter = cent + 48;
    if(caracter != 48)
      Serial_Envia_Caracter(caracter);
    dez = ((level - cent*100)/10);
    caracter = dez + 48;
    Serial_Envia_Caracter(caracter);
    caracter = (level%10) + 48;
    Serial_Envia_Caracter(caracter);
    Serial_Envia_Caracter('%');    
    
    Serial_Envia_Caracter(10);
    Serial_Envia_Caracter(13);
}



void Transmite_RAM_Ocupada(void)
{
    INT8U caracter = 0;
    INT8U mil,cent,dez;
    INT16U SPAddress = 0;
   
    Serial_Envia_Frase("MEMORY: ");
    
    UserEnterCritical();
    SPAddress = iStackAddress;
    UserExitCritical();
    
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
      Serial_Envia_Caracter('[');      
      Serial_Envia_Caracter(j+48);
      Serial_Envia_Frase("] ");
      Serial_Envia_Frase(ContextTask[j].TaskName);
      Serial_Envia_Frase(": ");
      
      UserEnterCritical();
      VirtualStack = ContextTask[j].StackInit - ContextTask[j].StackPoint;
      UserExitCritical();
      
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
    
    Serial_Envia_Caracter('[');      
    Serial_Envia_Caracter(j+48);
    Serial_Envia_Frase("] ");

    Serial_Envia_Frase("Idle Task: ");
    
    UserEnterCritical();  
    VirtualStack = ContextTask[NUMBER_OF_TASKS+1].StackInit - ContextTask[NUMBER_OF_TASKS+1].StackPoint;
    UserExitCritical();
      
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




#if (COMPUTES_CPU_LOAD == 1)
void Transmite_CPU_Load(void)
{
    INT16U percent = 0;
    INT8U caracter = 0;
   
    UserEnterCritical();
    percent = LastOSDuty;
    percent = (percent*100)/TIMER_MODULE;
    UserExitCritical();   
   
    Serial_Envia_Frase("CPU LOAD: ");
    
    if (percent >= 100)
    {
      Serial_Envia_Frase("100 %");
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);      
    }
    else
    {
      caracter = (percent / 10) + 48;
      Serial_Envia_Caracter(caracter);
      caracter = (percent%10) + 48;
      Serial_Envia_Caracter(caracter);
      Serial_Envia_Caracter('%');
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);
    }
}
#endif



void Reason_of_Reset(void)
{  
  INT8U reason = 0;
  
  reason = SRS;
  switch(reason)
  {
    case 0b00000000:
      Serial_Envia_Frase("Reset caused by BDM");
      break;
      
    case 0b00000010:
      Serial_Envia_Frase("Reset caused by low voltage");
      break;      
      
    case 0b00001000:
      Serial_Envia_Frase("Reset caused by illegal address");
      break;
      
    case 0b00010000:
      Serial_Envia_Frase("Reset caused by illegal opcode");
      break;
      
    case 0b00100000:
      Serial_Envia_Frase("Reset caused by watchdog");
      break;
      
    case 0b01000000:
      Serial_Envia_Frase("Reset caused by reset pin");
      break;
      
    case 0b10000010:
    case 0b10000000:
      Serial_Envia_Frase("Power-on Reset (Cold reset)");
      break;
      
    default:
      SerialReset();
      break;      
  }
}


#if (OSTRACE == 1) 
/* OS trace function */
/* coded as 8 bit: 0xTTTTTSSS 
- T: task bits (1-31: task id, 0: ISR) 
- S: syscall (
0: DELAYTASK, 1: SEMPEND, 2: QUEUEPEND, 3: MUTEXPEND
4: SEMPOST, 5: QUEUEPOST, 6: MUTEXPOST, 7: OS_IDLE
0x00: undefined
*/

static INT8U OSTrace[MAX_DEPTH_TRACE];
static INT8U OSTrace_snapshot[MAX_DEPTH_TRACE];
static INT8U index;


// update OSTrace history
void Update_OSTrace (INT8U task, INT8U syscall)
{   
  OSTrace[index] = (INT8U)((task << 5) | syscall);
  
  index++; 
  if(index == MAX_DEPTH_TRACE)
  {
    index=0;
  }
}

// send OSTrace to serial
void Send_OSTrace(void){
  
  #if (NESTING_INT == 1)
  INT16U CPU_SR = 0;
  #endif
  INT8U i=0;
  INT8U j=0;
  const CHAR8 *TaskName;
  
  OSEnterCritical();
  for(i=index, j=0;i<MAX_DEPTH_TRACE;i++,j++){
    OSTrace_snapshot[j] = OSTrace[i];
    OSTrace[i] = 0; // clear history
  }
  
  for(i=0;i<index;i++,j++){
    OSTrace_snapshot[j] = OSTrace[i];
    OSTrace[i] = 0; // clear history
  }
  index = 0;  
  OSExitCritical();
  
  j=0;
  do{      
    for(i=j;i<(j+MAX_TRACE_LINE);i++)
    {
        if(!(OSTrace_snapshot[i]>>5))
        {
            switch(OSTrace_snapshot[i] & 0x07)
            {
              case ISR_ONLY:
                Serial_Envia_Frase("ISR ONLY"); 
                break;         
              case ISR_TICK:
                #if(OS_TICK_SHOW == 1)  
                  Serial_Envia_Frase("ISR TICK"); 
                #endif
                break;
              case SEMPOST:
                Serial_Envia_Frase("ISR SEMPOST");        
                break;
              case QUEUEPOST:
                Serial_Envia_Frase("ISR QUEUEPOST");
                break;
              case MUTEXPOST:
                Serial_Envia_Frase("ISR MUTEXPOST");
                break;
              case OS_IDLE:
                #if(OS_IDLE_SHOW == 1)
                  Serial_Envia_Frase("ISR IDLE");
                #endif
                break; 
              default:
                Serial_Envia_Frase("________");
                break;
            }
        }else
        {
           #if(OS_IDLE_SHOW == 1)  
            Serial_Envia_Frase("TASK ");
            car = OSTrace_snapshot[i]>>5;
            car = (car / 10) + 48;
            Serial_Envia_Caracter(car);
            car = OSTrace_snapshot[i]>>5;
            car = (car%10) + 48;
            Serial_Envia_Caracter(car);
            Serial_Envia_Caracter(' ');
          #else
           if((OSTrace_snapshot[i]>>5) != (NUMBER_OF_TASKS+1))
           {
              TaskName = ContextTask[OSTrace_snapshot[i]>>5].TaskName;
              Serial_Envia_Frase(TaskName);
              Serial_Envia_Caracter(' ');
           }
          #endif  
            
            switch(OSTrace_snapshot[i] & 0x07){
              case DELAYTASK:
                Serial_Envia_Frase("DELAYTASK");
                break;
              case SEMPEND:
                Serial_Envia_Frase("SEMPEND");
                break;
              case QUEUEPEND:
                Serial_Envia_Frase("QUEUEPEND");
                break;
              case MUTEXPEND:
                Serial_Envia_Frase("MUTEXPEND");
                break;
              case SEMPOST:
                Serial_Envia_Frase("SEMPOST");        
                break;
              case QUEUEPOST:
                Serial_Envia_Frase("QUEUEPOST");
                break;
              case MUTEXPOST:
                Serial_Envia_Frase("MUTEXPOST");
                break;
              case OS_IDLE:
                #if(OS_IDLE_SHOW == 1)
                  Serial_Envia_Frase("IDLE");
                #endif
                break; 
              default:
                break;
            }
        }

        Serial_Envia_Frase(" - ");
    }   
    j += MAX_TRACE_LINE;
    Serial_Envia_Caracter(LF); 
  } while(j < MAX_DEPTH_TRACE);

  Serial_Envia_Caracter(CR);

}

#endif

