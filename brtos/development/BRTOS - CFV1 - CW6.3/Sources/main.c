#include "hardware.h"
#include <stdlib.h>

#include "BRTOS.h"
#include "tasks.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"
#include "flash.h"
#include "glcd.h"
#include "graphic.h"


#ifdef __cplusplus
 extern "C"
#endif



/*Protótipos de Funcoes*/ 
void MCU_init(void); /* Device initialization function declaration */



// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer;


// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue  *Serial;
INT8U         debug_display = 0;
INT8U         flash_status = 0;

// Declara uma estrutura de fila para o AD
OS_QUEUE_16   ADBuffer;
BRTOS_Queue   *AD;

INT16U        test = 0;
INT32U        test32 = 0;
INT8U         returns = 0;

BRTOS_Sem     *SemTeste;

// Declara uma estrutura de fila para o AD
OS_QUEUE_32   Buffer32;
BRTOS_Queue   *Q32;



void main(void)
{
  // call Device Initialization
  MCU_init();

  // Initialize BRTOS
  BRTOS_Init(); 
  
  // Cria um gerenciador de eventos p/ a porta serial
  // onde os dados gerenciados por este evento
  // estão em buffer circular, chamado de SerialPortBuffer  
  
  if (OSSemCreate(0,&SemTeste) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  

  if (OSQueue16Create(&ADBuffer,32, &AD) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  }; 

  if (OSQueueCreate(&SerialPortBuffer,128, &Serial) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if (OSQueue32Create(&Buffer32,16, &Q32) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };  
    
  
  // 16 bit queue test
  OSWQueue16(&ADBuffer,9876);
  OSWQueue16(&ADBuffer,5432);
  OSWQueue16(&ADBuffer,9821);                  
  
  returns = OSRQueue16(&ADBuffer, &test);
  returns = OSRQueue16(&ADBuffer, &test);  
  returns = OSRQueue16(&ADBuffer, &test); 
  

  // 16 bit queue test
  OSWQueue32(&Buffer32,0x11223344);
  OSWQueue32(&Buffer32,0x55667788);
  OSWQueue32(&Buffer32,0x11225566);   
  
  returns = OSRQueue32(&Buffer32, &test32);
  returns = OSRQueue32(&Buffer32, &test32);  
  returns = OSRQueue32(&Buffer32, &test32);  

    
  ////////////////////////////////////////////////////////////
  /////      Initialize Hardware                         /////
  ////////////////////////////////////////////////////////////  
  
  // Initialize Ports
  PortSetup();
  
  // Initialize A/D Converter
  ADSetup(HighSpeed, ShortSampleTime, 12);

  // PWM Module Select
  // PWM = Frequency 10Khz
  // Center Align
  // Channell 0 = Duty 40%
  // Polarity = Positive
  // Channell 1 = Duty 40%
  // Polarity = Negative
  PWMSetup(2,10000,CenterAlign,1,40,NegativeDutyCyclePolarity,1,40,PositiveDutyCyclePolarity);
  

  // Inicia a porta serial a 19200 bauds
  // Prioridade máxima a utilizar o recurso = 4
  init_SCI2(4);
  
  //////////////////////////////////////////////////////////
  // Utilizado no DEBUG para verificar motivo do Reset   ///
  //////////////////////////////////////////////////////////
  //SerialReset();
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////  
  
  /*
  // Initialize Display LCD
  init_lcd();
  // Prioridade máxima a utilizar o recurso = 16
  init_lcd_resource(16);
  */

  // Init GLCD  
  //glcd_init(ON);
    
  // Initialize GLCD Mutex resource with max priority = 16
  //init_glcd_resource(16);
  
  

  
  /*
  // Initialize FLASH EEPROM Emulation
  init_FLASH();
  
  // Apaga uma página = 1kb - Faixa de 0x1F800 a 0x1FBFF
  flash_status = flash_page_erase(0x1F800);
  
  // Escreve um valor de 32 bits no endereço 0x1F800 da flash
  flash_status = flash_write(0x1F800,0x12345678);
  
  // Escreve um valor de 32 bits no endereço 0x1F804 da flash
  flash_status = flash_write(0x1F804,0xAABBCCDD);
  
  teste1 = flash_read(0x1F804);
  teste2 = flash_read(0x1F800);
  teste3 = flash_read(0x1F808);
  
  write_Flash(0x1F80C, "Teste de Escrita");
  */
  
 
  /////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////  
  
  
  // Instala todas as tafefas no seguinte formato:
  // (Endereço da tarefa, Nome da tarefa, Número de Bytes do Stack Virtual,
  //  Prioridade da Tarefa)
  
  // OBS: The number of installed tasks must be supplied in myRTOS.c
  
  // Cada tarefa deve possuir uma prioridade
  // A instalação de uma tarefa em um prioridade ocupada gerará um código de exceção
  

  if(InstallTask(&System_Time,"System Time",100,31) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
     
  
  if(InstallTask(&Task_Serial,"Serial Handler",200,3) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  
  if(InstallTask(&Test_Task_1,"Tarefa de Teste 1",200,6) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  
  if(InstallTask(&Test_Task_2,"Tarefa de Teste 2",200,10) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };    
  
  
  /*
  if(InstallTask(&Task_5,"Graphic LCD",220,15) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  */
            
        
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


