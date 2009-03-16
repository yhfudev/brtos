#include "OS_RTC.h"
#include "BRTOS.h"
#include <hidef.h> /* for EnableInterrupts macro */

// estrutura - Hora
  OSTime Hora;
  OSDate Data;

  // inicialização dos ponteiros 
  // para as estruturas - Hora, Dia e Dia Completo (Dia + Hora)   
  // Ptr_Hora = &Hora_Exata;  
  // Ptr_Dia = &Dia_Exato;  
  // Ptr_Dia_Completo = &Dia_Completo;  	
	
// Exemplo de uso no RTOS

// void RTC_Task(void){
   
   /* task setup */
   
//  estruturas - Hora, Dia e Dia Completo (Dia + Hora)
//  sHora_Exata Hora;   
//  sDia_Exato Dia;     

  
//  RTC_Reset_Hora(&Hora);
//  RTC_Reset_Dia(&Dia);
  
  
   /* task main loop */
   
//   for (;;)
//   {
//      DelayTask(TEMPO_1SEG_TICKS);
      
//      Atualiza_RTC_Hora(&Hora);
//      Atualiza_RTC_Dia(&Dia);
//   }
// }



void OSUpdateTime(OSTime *Ptr_Hora)
{

	
  	OSEnterCritical;
  	Ptr_Hora -> RTC_Second++;

  	if (Ptr_Hora -> RTC_Second == 60){

  		Ptr_Hora -> RTC_Second = 0;
  		Ptr_Hora -> RTC_Minute++;

  	if (Ptr_Hora -> RTC_Minute == 60){

  		Ptr_Hora -> RTC_Minute = 0;
  		Ptr_Hora -> RTC_Hour++;

  	if (Ptr_Hora -> RTC_Hour == 24){

  		Ptr_Hora -> RTC_Hour = 0;
  		
  	}}}
  	
  	OSExitCritical;
	
}

void OSUpdateUptime(OSTime *Ptr_Hora,OSDate *Ptr_Dia)
{

	
  	OSEnterCritical;
  	Ptr_Hora -> RTC_Second++;

  	if (Ptr_Hora -> RTC_Second == 60){

  		Ptr_Hora -> RTC_Second = 0;
  		Ptr_Hora -> RTC_Minute++;

  	if (Ptr_Hora -> RTC_Minute == 60){

  		Ptr_Hora -> RTC_Minute = 0;
  		Ptr_Hora -> RTC_Hour++;

  	if (Ptr_Hora -> RTC_Hour == 24){

  		Ptr_Hora -> RTC_Hour = 0;
  		Ptr_Dia -> RTC_Day++;
  		
  	}}}
	  OSExitCritical;
}


void OSUpdateDate(OSDate *Ptr_Dia){   

	
	OSEnterCritical;
	Ptr_Dia -> RTC_Day++;	
		
	if (Ptr_Dia -> RTC_Day == 30){ 
	// deve-se adaptar para os dias exatos de cada mês

		Ptr_Dia -> RTC_Day = 0;
		Ptr_Dia -> RTC_Month++;

	if (Ptr_Dia -> RTC_Month == 12){

		Ptr_Dia -> RTC_Month = 0;
		Ptr_Dia -> RTC_Year++;

	if (Ptr_Dia -> RTC_Year == 9999){    // ano máximo 9999	

		Ptr_Dia -> RTC_Year = 0;
	}}}
	OSExitCritical;
	
}


/*
void OSUpdateUptime(OSTime *Ptr_Hora,OSDate *Ptr_Dia)
{
	   OSUpdateTime(Ptr_Hora);
     OSUpdateDate(Ptr_Dia);
}
*/



void OSResetTime(OSTime *Ptr_Hora)
 {
      
   OSEnterCritical;
   Ptr_Hora->RTC_Second = 0;
   Ptr_Hora->RTC_Minute = 0;
   Ptr_Hora->RTC_Hour = 0;
   OSExitCritical;
      
 }    
 
void OSResetDate(OSDate *Ptr_Dia)
 {
   
   OSEnterCritical;
   Ptr_Dia->RTC_Day = 0;
   Ptr_Dia->RTC_Month = 0;
   Ptr_Dia->RTC_Year = 0;
   OSExitCritical;
      
 }
 
 
OSTime OSUptime(void)
{
  return Hora;
}

OSDate OSUpDate(void)
{
  return Data;
}
  
 
/*
 
void RTC_Reset(sDia_Completo *Ptr_Dia_Completo)
 {
   
   
   RTC_Reset_Hora(Ptr_Dia_Completo -> DC_Hora_Exata);
   RTC_Reset_Dia(Ptr_Dia_Completo -> DC_Dia_Exata);
      
 }   
 
 */