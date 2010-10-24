

#include "BRTOS/includes/BRTOS.h"
#include "BRTOS/includes/mcu_init.h"



/** init CPU function */
/** função de início da CPU */
void mcu_init(void);

/** declara um ponteiro do evento semáforo */
/** semaphore event pointer */
BRTOS_Sem *SEMTESTE;

/** Declara evento da serial */
BRTOS_Queue *Serial;

/** Declara uma estrutura de fila para a porta serial */
OS_QUEUE SerialPortBuffer;

/**
Main function
*/

void main(void) {
    
  mcu_init();
  
  BRTOS_Init();
  
  init_UART(4); /** assign mutex priority 4 */
  Serial_Envia_Frase("Serial iniciada com prioridade 4");
  Serial_Envia_Caracter(LF);
  Serial_Envia_Caracter(CR);
  
  /** Cria evento de semáforo - contador inicial 0 */
  /** Semaphore event - counter 0 */
  if (OSSemCreate(0,&SEMTESTE)) { while(1){}; }  
  
  /** Fila da porta serial - 16 bytes */
  if (OSQueueCreate(&SerialPortBuffer,16, &Serial) != ALLOC_EVENT_OK)  { while(1){}; }
 
  /** Cada tarefa possui um contexto de 28 bytes */
  /** Instalação da tarefa 1 - prioridade 15 - stack 32 bytes + contexto */
  if(InstallTask(&System_Time,"System Time",32,15) != OK) { while(1){}; }     
  
  /** Instalação da tarefa 2 - prioridade 3 - stack 32 bytes + contexto */
  if(InstallTask(&Task_Serial,"Serial Handler",32,3) != OK) { while(1){}; }
  
  /** Instalação da tarefa 3 - prioridade 2 - stack 32 bytes + contexto */
  if(InstallTask(&Task_1,"Task1",32,2) != OK) { while(1){}; }

  /** Instalação da tarefa 4 - prioridade 1 - stack 32 bytes + contexto */
  if(InstallTask(&Task_2,"Task2",32,1) != OK) { while(1){}; }
  
  /** Start Task Scheduler */
  /** Inicia OS */
  if(TaskStartScheduler() != OK) { while(1){}; }
  
  /** nunca chega aqui */
  /** never gets here */
  
 }

