#ifndef OS_RTC_H
#define OS_RTC_H

#include "OS_types.h"


typedef struct {

	INT8U RTC_Second;
	INT8U RTC_Minute;
	INT8U RTC_Hour;	

} OSTime; // *Ptr_Hora


typedef struct {

	INT8U RTC_Day;
	INT8U RTC_Month;
  INT16U RTC_Year;
	
} OSDate; 

// *Ptr_Dia


typedef struct {


	OSTime DC_Hora_Exata;
	OSDate  DC_Dia_Exato;

} OSTime_Date;  // *Ptr_Dia_Completo 
  


// void Atualiza_RTC(sDia_Completo *Ptr_Dia_Completo);

void OSUpdateTime(OSTime *Ptr_Hora);

void OSUpdateDate(OSDate *Ptr_Dia); 

void OSResetTime(OSTime *Ptr_Hora);
 
void OSResetDate(OSDate *Ptr_Dia);

void OSUpdateUptime(OSTime *Ptr_Hora,OSDate *Ptr_Dia);

OSTime OSUptime(void);

OSDate OSUpDate(void);
 
// void RTC_Reset(sDia_Completo *Ptr_Dia_Completo);

// Declaração de variáveis (não utilizada)

// estruturas - Hora, Dia e Dia Completo (Dia + Hora)
// sHora_Exata Hora_Exata;   
// sDia_Exato Dia_Exato;    
// sDia_Completo Dia_Completo;

// ponteiros para as estruturas - Hora, Dia e Dia Completo (Dia + Hora)
// sHora_Exata *Ptr_Hora = &Hora_Exata; 
// sDia_Exato *Ptr_Dia = &Dia_Exato; 
// sDia_Completo *Ptr_Dia_Completo = &Dia_Completo; 

extern OSTime Hora;
extern OSDate Data;


#endif
