#include <hidef.h> /* for EnableInterrupts macro */
#include <stdlib.h>
#include "derivative.h" /* include peripheral declarations */

#include "myRTOS.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"

#ifdef __cplusplus
 extern "C"
#endif

// Para habilitar a alimentação do WatchDog
#define COP 1


/*Protótipos de Funcoes*/
void MCU_init(void); /* Device initialization function declaration */
void PortSetup(void);
void SerialReset(void);
static void Task_1(void);
static void Task_2(void);
static void Task_3(void);
static void Task_4(void);
static void Task_5(void);
static void Task_6(void);


INT8S VetorSerial[12];
INT8U contador=0;
INT8U reset1 = 0;
INT8U reset2 = 0;

// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer;

// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOSEvent *Serial;
BRTOSEvent *Teclado;
BRTOSEvent *ConversorAD;
INT8U debug_display = 0;

// Número de entradas na interrupção de IRQ
INT16U iIRQ = 0;


void main(void) {
  
  MCU_init(); /* call Device Initialization */

  /* include your code here */
  
  // Declara o tabela de blocos de controle de Eventos
  initEvents();
  
  // Cria um gerenciador de eventos p/ a porta serial
  // onde os dados gerenciados por este evento
  // estão em buffer circular, chamado de SerialPortBuffer
  
  if (OSQCreate(&SerialPortBuffer,150,&Serial) == ALLOC_EVENT_OK)
  {
    // Não consegui retornar o ponteiro pela função
    // Resolvi desta forma p/ acessar os dados do controle de evento criado
    //Serial = &OSEventTbl[iBRTOSEvent-1];
  }
  
  
  if (OSSemCreate(0,&Teclado) == ALLOC_EVENT_OK)
  {
    // Não consegui retornar o ponteiro pela função
    // Resolvi desta forma p/ acessar os dados do controle de evento criado
    //Teclado = &OSEventTbl[iBRTOSEvent-1];
  }
  
  if (OSMboxCreate((void *)0,&ConversorAD) == ALLOC_EVENT_OK)
  {
    // Não consegui retornar o ponteiro pela função
    // Resolvi desta forma p/ acessar os dados do controle de evento criado
    //ConversorAD = &OSEventTbl[iBRTOSEvent-1];
  }  
  
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
  PWM3Setup(10000,CenterAlign,1,40,NegativeDutyCyclePolarity,1,40,PositiveDutyCyclePolarity);

  // Inicia a porta serial a 19200 bauds
  init_SCI2();
  
  //////////////////////////////////////////////////////////
  // Utilizado no DEBUG para verificar motivo do Reset   ///
  //////////////////////////////////////////////////////////
  SerialReset();
  //////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////  
  
  
  // Initialize SPI 1 Port
  init_SPI1();

  // Initialize Display LCD
  init_lcd();
  init_lcd_resource();
  
  // Falta inicializar o radio IEEE 802.15.4
  //MRF24J40Init();

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
  
  if(InstallTask(&Task_3,"Stack Serial",100,3) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if(InstallTask(&Task_4,"Teclado e PWM",100,2) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  if(InstallTask(&Task_5,"Display LCD",100,4) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };  
  
  //if(InstallTask(&Task_6,"Teste de MailBox",100,5) != OK)
  //{
    // Oh Oh
    // Não deveria entrar aqui !!!
  //  while(1){};
  //};    
        
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


static void Task_1(void)
{
  /* task setup */
  OSResetTime(&Hora);
  /* task main loop */
  
  for (;;) 
  {
      OSUpdateUptime(&Hora,&Data);
      PTHD_PTHD0 = ~PTHD_PTHD0;
      //SPISendChar('A');
      DelayTask(1000);
  }
}


static void Task_2(void)
{
   /* task setup */
  
   /* task main loop */
   for (;;)
   {
      PTHD_PTHD1 = ~PTHD_PTHD1;
      #ifdef COP
        __RESET_WATCHDOG();
      #endif
      DelayTask(10);
   }
}


static void Task_3(void)
{
   /* task setup */
   INT16U i=0;
  
   /* task main loop */
   for (;;) 
   {
      DelayTask(3);
      
      while(SerialPortBuffer.OSQEntries>0)
      {
        VetorSerial[i] = OSRQueue(&SerialPortBuffer);
        i++;
        if (i == 3)
        {
          i = 0;
          switch(VetorSerial[0])
          {
            case 49:
              acquireSCI2();
              Transmite_CPU_Load();
              releaseSCI2();
              break;
            case 50:
              acquireSCI2();
              Transmite_Uptime();
              releaseSCI2();
              break;
            case 51:
              acquireSCI2();
              Serial_Envia_Frase("myRTOS Version 0.8");
              Serial_Envia_Caracter(10);
              Serial_Envia_Caracter(13);
              releaseSCI2();
              break;
            case 52:
              acquireSCI2();
              Transmite_RAM_Ocupada();
              releaseSCI2();
              break;
            case 53:
              acquireSCI2();
              Transmite_Task_Stacks();
              releaseSCI2();
              break;
            case 54:
              acquireSCI2();
              Transmite_Duty_Cycle();            
              releaseSCI2();
              break;              
            default:
              acquireSCI2();
              Serial_Envia_Frase("Codigo Invalido");
              Serial_Envia_Caracter(10);
              Serial_Envia_Caracter(13);
              releaseSCI2();
              break;
          }
        }
      }      
   }
}


static void Task_4(void)
{
   /* task setup */   
   INT8U tecla=0;
   INT16S correcao = 0;

  
   /* task main loop */
   for (;;)
   {
   
      // Stop Task to wait button click
      OSSemPend (Teclado,0);
      // Espera por estabilização do botão
      DelayTask(50);      
        // Módule = 1200
      correcao = TPM3C1V;
      tecla = PTDD & 0x0F;
      
      if (tecla == 0x0E)
      {
        correcao = correcao + 24;
        if (correcao > 1150)
          correcao = 1150;
        TPM3C1V = correcao;
      }
        
      if (tecla == 0x0D)
      {
        correcao = correcao - 24;
        if (correcao < 50)
          correcao = 50;
        TPM3C1V = correcao;
      }
      
      
      if (tecla == 0x0B)
      {
        if (debug_display < 2)
          debug_display = debug_display ^ 1;
      }
      
      if (tecla == 7)
      {
          debug_display = 2;  
      }          
      
      // Evita problemas com mais de uma interrupção de teclado seguida
      KBI2SC_KBACK = 1;
      KBI2SC_KBIE = 1;
      KBI2SC_KBACK = 1;
   }
}

static void Task_5(void)
{
   /* task setup */
   INT16U percent = 0;
   INT8U caracter = 0;
   
   
   acquireLCD();
   posiciona(1,1);
   write_lcd("MyRTOS 0.8");
   releaseLCD();
   //posiciona(2,1);
   //write_lcd("Duty Cycle: ");
  
   /* task main loop */
   for (;;) 
   {
      
      //posiciona(2,13);
      
      if (debug_display == 0)
      {
        OSEnterCritical;
        percent = (TPM3C1V*100)/TPM3MOD;
        OSExitCritical;    
    
        acquireLCD();
        posiciona(2,1);
        write_lcd("Duty Cycle: ");    
        caracter = (percent / 10) + 48;
        caractere(caracter);
        caracter = (percent%10) + 48;
        caractere(caracter);
        caractere('%');
        caractere(' ');
        releaseLCD();
      }

      if (debug_display == 1)
      {      
        acquireLCD();
        posiciona(2,1);
        Transmite_Uptime_Display();
        releaseLCD();
      }
      
      
      
      if(debug_display == 2)
      {
        // Initialize Display LCD
        acquireLCD();
        init_lcd();      
        posiciona(1,1);
        write_lcd("MyRTOS 0.8");
        posiciona(2,1);
        write_lcd("Duty Cycle: ");
        releaseLCD();
        debug_display = 0;
      }
      
      /*
      Serial_Envia_Frase("Teste");
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);
      
      */
      
      DelayTask(200);
   }
}

// Antiga tarefa 5

   //INT16U atualiza_pwm = 0;

      //DelayTask(10);   
      //atualiza_pwm = converte(2);
      //Mensagem = atualiza_pwm;
      //TPM3C0V = atualiza_pwm >> 2;
      
      // Exemplo de MailBox Post
      //OSMboxPost(ConversorAD,(void *)&Mensagem);


static void Task_6(void)
{
  /* task setup */
  INT16U recebe;
  
  /* task main loop */
  
  for (;;)
  {
      // Exemplo de MailBox Pend
      //recebe = *(INT16U *)OSMboxPend(ConversorAD,(INT16U)1000);
  }
}



void PortSetup(void)
{
  PTHD = 1;
  PTHDD = 0x03;
  PTHDS = 0x03;
}

void SerialReset(void)
{
  acquireSCI2();
  reset1 = SRS/100;
  Serial_Envia_Caracter(reset1+48);
  
  reset2 = ((SRS - reset1*100)/10);
  Serial_Envia_Caracter(reset2+48);
  
  reset1 = ((SRS - reset1*100 - reset2*10)%10);
  Serial_Envia_Caracter(reset1+48);
  Serial_Envia_Caracter(10);
  Serial_Envia_Caracter(13);
  releaseSCI2();
}


