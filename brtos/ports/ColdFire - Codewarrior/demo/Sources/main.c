#include <hidef.h> /* for EnableInterrupts macro */
#include <stdlib.h>
#include "derivative.h" /* include peripheral declarations */

#include "myRTOS.h"
#include "tasks.c"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"
#include "MRF24J40.h"


#ifdef __cplusplus
 extern "C"
#endif


/*Protótipos de Funcoes*/
void MCU_init(void); /* Device initialization function declaration */


//INT8S VetorSerial[12];
INT8U contador=0;


// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer;

// Declara um ponteiro para o bloco de controle da Porta Serial
OS_EVENT *Serial;
OS_EVENT *Teclado;
OS_EVENT *RF_Event;
OS_EVENT *ConversorAD;
INT8U debug_display = 0;



void main(void) {
  
  MCU_init(); /* call Device Initialization */

  /* include your code here */

  
  ////////////////////////////////////////////////////////////
  /////      Initialize Events                           /////
  ////////////////////////////////////////////////////////////
  
  // Declara o tabela de blocos de controle de Eventos
  initEvents();
  
  // Cria um gerenciador de eventos p/ a porta serial
  // onde os dados gerenciados por este evento
  // estão em buffer circular, chamado de SerialPortBuffer
  
  if (OSQCreate(&SerialPortBuffer,512,&Serial) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  
  if (OSSemCreate(0,&Teclado) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if (OSMboxCreate((void *)0,&ConversorAD) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  ////////////////////////////////////////////////////////////
      

  ////////////////////////////////////////////////////////////
  /////      Initialize Hardware                         /////
  ////////////////////////////////////////////////////////////
  
  // Initialize Tick Timer
  TickTimerSetup();
  
  // Initialize Ports
  PortSetup();
  
  // Initialize Keyboard
  Keyb2Setup(0x0F);
  
  // Initialize A/D Converter
  ADSetup(HighSpeed, ShortSampleTime, 12);

  // PWM = Frequency 10Khz
  // Center Align
  // Channell 0 = Duty 40%
  // Polarity = Positive
  // Channell 1 = Duty 40%
  // Polarity = Negative
  //PWM3Setup(10000,CenterAlign,1,40,NegativeDutyCyclePolarity,1,40,PositiveDutyCyclePolarity);

  // Inicia a porta serial a 19200 bauds
  // Prioridade máxima a utilizar o recurso = 3
  init_SCI2(3);
  
  //////////////////////////////////////////////////////////
  // Utilizado no DEBUG para verificar motivo do Reset   ///
  //////////////////////////////////////////////////////////
  SerialReset();
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////  
  

  // Initialize Display LCD
  init_lcd();
  // Prioridade máxima a utilizar o recurso = 5
  init_lcd_resource(5);
  
  
  ////////////////////////////////////////////////
  //     Initialize IEEE 802.15.4 radio     //////
  ////////////////////////////////////////////////
  
  // Initialize SPI 1 Port
  init_SPI1();
  
  // Initialize interrupt and I/O radio port
  init_radio_port();
  
  // Initialize radio variables
  radio_init();
  
  // Inilitalize radio registers
  MRF24J40Init();
 
  /////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////  
  
 
  // Initialize global variables
  PreInstallTasks();
  
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
  
  if(InstallTask(&Task_3,"Stack Serial",200,4) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if(InstallTask(&Task_4,"Teclado e PWM",100,3) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if(InstallTask(&Task_5,"RF Module Mon",100,5) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };  
  
  if(InstallTask(&Task_6,"RF Event Handler",400,2) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
     while(1){};
  };    
        
  // Exemplo:
  // Como acessar o ponteiro de dados do evento:
  // No caso de uma fila, declara-se um ponteiro do tipo queue
  // p/ acessar os dados, iguala-se este ponteiro a OSEvent->OSEventPtr
  //Teste = Serial->OSEventPtr;

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
