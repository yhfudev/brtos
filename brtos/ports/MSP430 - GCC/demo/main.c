#include "hardware.h"
#include "OS_types.h"
#include "BRTOS.h"
#include "event.h"
#include "queue.h"
#include "drivers.h"



// prot�tipos de fun��es

/**
CPU init function.
*/
void mcu_init(void);
void Task_1(void);
void Task_2(void);



// Declara uma estrutura de fila
BRTOS_Queue SerialPortBuffer;

// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue  *Serial;
// Declara um ponteiro para o bloco de controle de sem�foro
BRTOS_Sem *SEMTESTE;


/**
Main function with some blinking leds
*/
int main(void) {
    
	
  mcu_init();
  
  initEvents();
	
    // Initialize Tick Timer
  TickTimerSetup();
  
 
  // Initialize global variables
  PreInstallTasks();
  
  
  OSSemCreate(0,&SEMTESTE);
  
  
  if (OSQueueCreate(&SerialPortBuffer,32,&Serial) != ALLOC_EVENT_OK)
  {
    
    while(1){};
  }  
  
  
  UART_init(1);
  
  
  // Instala todas as tafefas no seguinte formato:
  // (Endere�o da tarefa, Nome da tarefa, N�mero de Bytes do Stack Virtual,
  //  Prioridade da Tarefa)
  
  // OBS: The number of installed tasks must be supplied in myRTOS.c
  
  // Cada tarefa deve possuir uma prioridade
  // A instala��o de uma tarefa em um prioridade ocupada gerar� um c�digo de exce��o
  
  if(InstallTask(&Task_1,"Task1",100,1) != OK) 
  {
    // Oh Oh
    // N�o deveria entrar aqui !!!
    while(1){};
  }

  if(InstallTask(&Task_2,"Task2",100,0) != OK)
  {
    // Oh Oh
    // N�o deveria entrar aqui !!!
    while(1){};
  }
  
 
	

  // Start Task Scheduler
  if(TaskStartScheduler() != OK)
  {
    // Oh Oh
    // N�o deveria entrar aqui !!!
    while(1){};
  }
  
  for(;;) {
    
  } /* loop forever */
  
  return 0;
  	

}



