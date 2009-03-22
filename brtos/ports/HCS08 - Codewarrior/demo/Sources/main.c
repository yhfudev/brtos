#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include <stdlib.h>

#include "BRTOS.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"
#include "tasks.c"

#ifdef __cplusplus
 extern "C"
#endif


/*Protótipos de Funcoes*/
void MCU_init(void); /* Device initialization function declaration */

void main(void) {
  MCU_init(); /* call Device Initialization */

  /* include your code here */

  // Declara o tabela de blocos de controle de Eventos
  initEvents();
  
  // Initialize Tick Timer
  TickTimerSetup();
 
  // Initialize TPM2
  init_TPM2();

  // Initialize global variables
  PreInstallTasks();
  
  
  PTAD = 1;
  PTADD = 255;
  
  // Instala todas as tafefas no seguinte formato:
  // (Endereço da tarefa, Nome da tarefa, Número de Bytes do Stack Virtual,
  //  Prioridade da Tarefa)
  
  // OBS: The number of installed tasks must be supplied in myRTOS.c
  
  // Cada tarefa deve possuir uma prioridade
  // A instalação de uma tarefa em um prioridade ocupada gerará um código de exceção
  
  if(InstallTask(&Task_1,"System Uptime",80,1) != OK) 
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  }; 

  if(InstallTask(&Task_2,"Feed WatchDog",50,0) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  
  if(InstallTask(&Task_3,"Tarefa Teste 1",50,2) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  }; 
  
  if(InstallTask(&Task_4,"Tarefa Teste 2",50,3) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  }; 
  

  // Start Task Scheduler
  if(TaskStartScheduler() != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };



  for(;;) {
    /* __RESET_WATCHDOG(); by default, COP is disabled with device init. When enabling, also reset the watchdog. */
  } /* loop forever */
  /* please make sure that you never leave main */
}


