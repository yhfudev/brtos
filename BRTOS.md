

&lt;BR&gt;


# 1. Introdução #

Atualmente um grande número de microcontroladores, integrados em diversos equipamentos, exercem um papel importante no dia a dia das pessoas. Despertar ao som de um CD Player programável, tomar café da manhã preparado por um microondas digital, e ir ao trabalho de carro, cuja injeção de combustível é microcontrolada, são apenas alguns exemplos.

O mercado de microcontroladores apresenta-se em franca expansão, ampliando seu alcance principalmente em aplicações residenciais, industriais, automotivas e de telecomunicações. Estima-se que, em 2010, em média uma pessoa interagirá com 250 dispositivos com microcontroladores diariamente.

Em um passado recente, o alto custo dos dispositivos eletrônicos limitou o uso dos microcontroladores apenas aos produtos domésticos considerados de alta tecnologia (televisão, vídeo e som). Porém, com a constante queda nos preços dos circuitos integrados, os microcontroladores passaram a ser utilizados em produtos menos sofisticados do ponto de vista da tecnologia, como máquinas de lavar, microondas, fogões e refrigeradores. Assim, a introdução do microcontrolador nestes produtos cria uma diferenciação e permite a inclusão de melhorias de segurança e de funcionalidade. Alguns mercados chegaram ao ponto de tornar obrigatório o uso de microcontroladores em determinados tipos de equipamentos, impondo um pré-requisito tecnológico.

Muitos produtos que temos disponíveis hoje em dia, simplesmente não existiriam, ou não teriam as mesmas funcionalidades sem um microcontrolador. É o caso, por exemplo, de vários instrumentos biomédicos, instrumentos de navegação por satélites, detetores de radar, equipamentos de áudio e vídeo, eletrodomésticos, entre outros. De maneira geral, os dispositivos microcontrolados e microprocessados executam um conjunto de tarefas pré-definidas, geralmente com requisitos específicos. Estes sistemas são conhecidos por Embedded Systems (Sistemas Embarcados).

Atualmente existem diversas plataformas para o desenvolvimento de sistemas embarcados microcontrolados, impulsionadas por diferentes fabricantes. Entre os principais fabricantes do setor estão: Freescale, Texas Instruments, Renesas Technology, Microchip e ST Microelectronics. Os periféricos e ambientes de programação para sistemas embarcados também apresentam este problema. Existem diversos fabricantes de sensores, displays, módulos RF, etc, competindo neste mercado que se encontra em franca expansão.

A utilização de um Sistema Operacional de Tempo Real provê ao desenvolvedor de sistemas embarcados um conjunto de ferramentas que além de facilitar o desenvolvimento, torna o código gerado mais portável, permitindo a mudança de plataforma caso seja necessário. Imagine que você possua um produto utilizando um microcontrolador que deixou de ser fabricado. Se o produto foi desenvolvido de forma não portável, o tempo de adaptação para outro microcontrolador disponível no mercado será maior.

Projetos de sistemas embarcados de menor complexidade ainda utilizam a linguagem de programação Assembly. Embora seja possível criar um código altamente otimizado, esta linguagem é extremamente dependente da CPU utilizada, além de não ser estruturada. Este tipo de linguagem dificulta muito a portabilidade do sistema desenvolvido para outra plataforma. Devido a este motivo, a linguagem de programação utilizada no projeto foi o C. Mesmo o código sendo desenvolvido em C, é possível que o código não seja 100% compatível com todos os ambientes de programação existentes no mercado.

O BRTOS foi desenvolvido com o intuito de ser um sistema operacional simples, com o mínimo de consumo de memória de dados e programa. Isto deve-se ao reduzido poder computacional e memória disponíveis nos microcontroladores de baixo custo de 8, 16 e 32 bits disponíveis no mercado. Quanto maior a quantidade de memória e periféricos de um microcontrolador, maior o seu custo. Através desta ferramenta será possível desenvolver aplicações mais organizadas e estruturadas, gerenciando o ambiente de tarefas concorrentes, inerente a sistemas embarcados, de forma eficiente.

## 1.1 Teste de Desempenho do BRTOS ##

O tempo de troca de contexto depende do clock e do microcontrolador utilizado.
Resultados obtidos até o momento:

Coldfire V1 - Troca de contexto em 4us com clock de barramento = 24Mhz.

&lt;BR&gt;


HCS08       - Troca de contexto em 5us com clock de barramento = 20Mhz.




&lt;BR&gt;


# 2. Como configurar e utilizar o BRTOS #


## 2.1 Configuração do BRTOS ##

A quantidade de RAM necessária para as tarefas dependerá da CPU escolhida (CPUs RISC necessitam maior espaço em RAM, pois possuem maior número de registradores), do número de interrupções habilitadas e das necessidades da tarefa (variáveis e chamadas de funções).

Antes de iniciar o desenvolvimento da aplicação é preciso configurar alguns parâmetros do sistema. Estas configurações devem ser realizadas nos arquivos **BRTOS.h** e **event.h**.

  * O arquivo BRTOS.h possui definições relativas a memória, clock e o microcontrolador utilizado;
  * Já o arquivo event.h define quais eventos / serviços do sistema serão utilizados (semáforos, mutex, mailbox e filas de mensagem).

O arquivo **BRTOS.h** apresentará a seguinte estrutura:

```
// Define if simulation or DEBUG
#define DEBUG 1

// Define if whatchdog active
#define COP 1

// Define CPU Stack Pointer Size
#define SP_SIZE 32

// Define the number of Task to be Installed
// must always be equal or higher to NumberOfInstalledTasks
#define NUMBER_OF_TASKS 6


// TickTimer Defines
#define configCPU_CLOCK_HZ          (INT32U)24008000
#define configTICK_RATE_HZ          (INT32U)1000
#define configTIMER_PRE_SCALER      0
#define TickCountOverFlow           (INT16U)32000


//Stack Defines
// P/ Coldfire com 8KB de RAM, limite de 5KB p/ STACK Virtual
#define HEAP_SIZE 5*1024


// Others Defines
#define configMAX_TASK_NAME_LEN 16
#define configMAX_TASK_INSTALL 32
```

  * **DEBUG** é utilizado em algumas plataformas que possuem diferenciações entre o simulador e o debugger. Desta forma o código se adapta a condição desejada. **DEBUG = 1** indica modo debugger / flash write.

  * **COP (Computer Operating Properly)** indica se o _watchdog_ do sistema estará ativo. **COP = 1** indica _watchdog_ ativo.

  * **SP`_`SIZE** é utilizado para informar ao compilador o tamanho do registrador _stack pointer_ do processador em bits. Para _Stack Pointer_ menor ou igual a 16 bits, utilize **SP`_`SIZE = 16**. Para _Stack Pointer_ menor ou igual a 32 bits, utilize **SP`_`SIZE = 32**.

  * **NUMBER`_`OF`_`TASKS** indica quantas tarefas podem ser instaladas em uma aplicação. O valor máximo para este _define_ é 31. Esta definição permite que uma menor quantidade de memória seja alocada para estrutura de contexto de tarefas. Utilize este recurso para reduzir o consumo de memória do sistema quando um número pequeno de tarefas for instalado.

  * **configCPU`_`CLOCK`_`HZ** indica a freqüência de barramento utilizada pelo microcontrolador em hertz.

  * **configTICK`_`RATE`_`HZ** define o _Timer Tick_ (marca de tempo) do sistema, ou seja, a resolução do gerenciador de tempo do RTOS. Valores entre 1ms (1000 Hz) e 10ms (100 Hz) são recomendados. Nunca esqueça que a resolução do gerenciamento de tempo é de + ou - 1 _Timer Tick_.

  * **configTIMER`_`PRE`_`SCALER** pode ser utilizado no _port_ do sistema para a configuração do hardware responsável pelo _Timer Tick_.

  * **TickCountOverFlow** determina o número máximo de marcas de tempo do sistema que podem ser utilizadas pelo gerenciador de tempo. Imagine o caso onde se utiliza um _Timer Tick_ de 1ms. Para gerar um _delay_ maior do que 32000ms pode ser necessário utilizar duas chamadas da função **DelayTask** ou utilizar a função **DelayTaskHMSM** que opera em milisegundos, segundos, minutos e horas.

  * **HEAP`_`SIZE** determina a quantidade de memória alocada como pilha virtual das tarefas. Sempre que uma tarefa é instalada, a quantidade de memória utilizada pela tarefa será alocada no _HEAP_.

  * **configMAX`_`TASK`_`NAME`_`LEN** configura o tamanho máximo em caracteres para o nome da tarefa. Por padrão do sistema, a associação de nomes às tarefas é desativada, devido ao consumo excessivo de memória.

  * **configMAX`_`TASK`_`INSTALL** determina o número máximo de tarefas que o escalonador do sistema pode gerenciar. O valor máximo deste _define_ é 32. Este valor indica ainda a posição de instalação da tarefa de espera (_Idle Task_). Quando se utiliza poucas tarefas, recomenda-se diminuir o valor deste _define_. Por exemplo, em uma aplicação com 6 tarefas, reduzir este valor p/ 7 limita a associação de prioridades disponíveis para as tarefas. Neste caso, as prioridades disponíveis seriam de 0 a 6, onde a prioridade 6 é ocupada pela _Idle Task_.
**OBS:** A partir da versão 1.1 do BRTOS quanto maior o valor de prioridade associado a uma tarefa, maior sua prioridade no sistema.

No arquivo **event.h** o desenvolvedor determinará quais serviços / eventos estarão disponíveis para a aplicação. Ainda, irá determinar a quantidade de instâncias possíveis para um determinado serviço, apresentando a seguinte estrutura:

```
// Habilita o serviço de semáforo do sistema
#define BRTOS_SEM_EN           1

// Habilita o serviço de mutex do sistema
#define BRTOS_MUTEX_EN         1

// Habilita o serviço de mailbox do sistema
#define BRTOS_MBOX_EN          1

// Habilita o serviço de filas do sistema
#define BRTOS_QUEUE_EN         1

// Define o número máximo de semáforos (limita a alocação de memória p/ semáforos)
#define BRTOS_MAX_SEM          4

// Define o número máximo de mutex (limita a alocação de memória p/ mutex)
#define BRTOS_MAX_MUTEX        4

// Define o número máximo de Mailbox (limita a alocação de memória p/ mailbox)
#define BRTOS_MAX_MBOX         1

// Define o número máximo de filas (limita a alocação de memória p/ filas)
#define BRTOS_MAX_QUEUE        1

// Define o número máximo de tarefas que podem ser acordadas pelo evento
#define WAIT_LIST_SIZE  4
```

  * Os _defines_ **BRTOS`_`SEM`_`EN**, **BRTOS`_`MUTEX`_`EN**, **BRTOS`_`MBOX`_`EN** e **BRTOS`_`QUEUE`_`EN** determinam se os serviços de semáforo, mutex, mailbox e filas, respectivamente, estarão ou não disponíveis para a aplicação. O valor 1 determina que o serviço estará disponível.

  * Os _defines_ **BRTOS`_`MAX`_`SEM**, **BRTOS`_`MAX`_`MUTEX**, **BRTOS`_`MAX`_`MBOX** e **BRTOS`_`MAX`_`QUEUE** determinam a quantidade máxima de instâncias que poderão ser criadas para os serviços de semáforo, mutex, mailbox e fila, respectivamente.

  * **WAIT`_`LIST`_`SIZE** determina a quantidade máxima de tarefas que pode ser suspensa por um determinado serviço. Imagine um caso onde várias tarefas compartilham um recurso (exemplo: display LCD). Quando uma tarefa adquire o direito de utilizar o recurso, as outras tarefas que o solicitarem serão suspensas, até que o recurso seja liberado. O recurso que gerencia recursos compartilhados é o **mutex**. O número máximo de tarefas que poderão ser suspensas por um mutex, por exemplo, é determinado pelo _define_ **WAIT`_`LIST`_`SIZE**.


## 2.2 Iniciando uma aplicação ##

A seqüência de inicialização do sistema deve seguir uma ordem bem determinada. Os seguintes passos devem ser realizados:
  1. Inicialização dos registradores principais do microcontrolador
  1. Inicialização dos serviços / eventos (caso sejam utilizados)
  1. Inicialização do hardware interno do microcontrolador
  1. Inicialização do hardware externo ao microcontrolador
  1. Inicialização das tarefas
  1. Inicialização do escalonador

A seguir é apresentado um exemplo de inicialização de uma aplicação utilizando o microcontrolador Freescale Coldfire V1:

```

// Declara ponteiros para os blocos de controle dos serviços / eventos do BRTOS

// Declara ponteiro p/ serviço de filas
BRTOS_Queue  *Serial;
// Declara ponteiro p/ serviço de semáforo
BRTOS_Sem    *Teclado;

// Declara ponteiro p/ serviço de caixa de mensagem
BRTOS_Mbox   *ConversorAD;


void main(void) {  
  ////////////////////////////////////////////////////////////
  /////      Inicialização dos registradores principais  /////
  /////      do microcontrolador utilizado               /////
  ////////////////////////////////////////////////////////////
  MCU_init();
                                  

  ////////////////////////////////////////////////////////////
  /////      Inicialização dos Serviços / Eventos        /////
  ////////////////////////////////////////////////////////////

  // Se algum serviço do sistema for utilizado, está função deve ser chamada
  // Declara o tabela de blocos de controle de Eventos
  initEvents();
  

  // Exemplo de criação de uma fila
  // Cria um gerenciador de eventos p/ a porta serial
  // onde os dados gerenciados por este evento
  // estão em buffer circular, chamado de SerialPortBuffer
  
  if (OSQueueCreate(&SerialPortBuffer,512,&Serial) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  

  // Exemplo de criação de um semáforo
  // No caso, o semáforo é utilizado para sincronizar a interrupção de teclado
  // com uma tarefa
  if (OSSemCreate(0,&Teclado) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
     
  
  // Exemplo de criação de uma caixa de mensagem
  // No caso será utilizado para acordar uma tarefa que está
  // a espera de um novo valor do conversor A/D
  if (OSMboxCreate(&ConversorAD,(INT16U *)0) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
  
  

  ////////////////////////////////////////////////////////////
  /////      Inicialização do Hardware                   /////
  ////////////////////////////////////////////////////////////
  
  // Inicializa o Tick Timer
  TickTimerSetup();
  
  // Inicializa as portas utilizadas na aplicação
  PortSetup();
  
  // Inicializa uma porta como teclado
  Keyb2Setup(0x0F);
  
  // Configura / Inicializa um conversor A/D
  ADSetup(HighSpeed, ShortSampleTime, 12);


  // Configura / Inicializa uma saída PWM
  // PWM = Frequency 10Khz
  // Center Align
  // Channell 0 = Duty 40%
  // Polarity = Positive
  // Channell 1 = Duty 40%
  // Polarity = Negative
  PWM3Setup(10000,CenterAlign,1,40,NegativeDutyCyclePolarity);

  // Configura / Inicia uma porta serial a 19200 bauds
  // Prioridade máxima a utilizar o recurso = 3
  // O protocolo utilizado no mutex do RTOS associado a este recurso
  // utiliza como parâmetro a maior prioridade das tarefas que irão utilizar
  // o recurso para evitar inversões de prioridade e deadlocks.
  init_SCI2(3);
  

  ////////////////////////////////////////////////////////////
  /////      Inicialização do Hardware Externo           /////
  ////////////////////////////////////////////////////////////

  // Inicializa um Display LCD 16x2
  init_lcd();
  // Prioridade máxima a utilizar o recurso = 5
  init_lcd_resource(5);
  
  
  ////////////////////////////////////////////////////////////
  /////      Inicialização das tarefas                   /////
  //////////////////////////////////////////////////////////// 

  // Função que inicializa as variáveis globais responsáveis
  // pelo gerenciamento de tarefas
  // Deve ser executada antes da instalação de qualquer tarefa
  PreInstallTasks();
  

  // Instala todas as tafefas no seguinte formato:
  // (Endereço da tarefa, Nome da tarefa, Número de Bytes do Stack Virtual,
  //  Prioridade da Tarefa)

  // Cada tarefa deve possuir uma prioridade
  // A instalação de uma tarefa em um prioridade ocupada gerará um código de exceção
  
  if(InstallTask(&Task_1,"System Uptime",100,1) != OK) 
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };

  if(InstallTask(&Task_2,"Feed WatchDog",100,0) != OK)
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
  
  
  if(InstallTask(&Task_6,"RF Event Handler",400,2) != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
     while(1){};
  };    
        

  ////////////////////////////////////////////////////////////
  /////    Inicialização do escalonador                  /////
  /////    A partir deste momento as tarefas instaladas  /////
  /////    começam a ser executadas                      /////
  //////////////////////////////////////////////////////////// 

  // Start Task Scheduler
  if(TaskStartScheduler() != OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
}
```

As tarefas da aplicação devem ser criadas no arquivo **tasks.c**. A seguir é apresentada a estrutura de uma tarefa:

```

//  Tarefa para gerenciamento de um RTC da aplicação

static void Task_RTC(void)
{
  // Configuração da tarefa
  // Este código será executado somente na inicialização do sistema
  OSResetTime(&Hora);


  // Loop principal da tarefa
  // Uma tarefa opera como um aplicativo independente,
  // utilizando o conceito de super laço ou laço infinito
  for (;;) 
  {
      // Código da tarefa
      OSUpdateUptime(&Hora,&Data);
      PTHD_PTHD0 = ~PTHD_PTHD0;
      DelayTask(1000);
  }
}
```


&lt;BR&gt;



## 2.3 Como determinar a prioridade associada a cada tarefa ##

Algumas regras podem ser seguidas para determinar prioridades de tarefas em um sistema operacional preemptivo baseado em prioridades. Algumas delas são:
  1. Tarefas que gerenciam dispositivos de I/O devem possuir alta prioridade. Um dos motivos é que estas tarefas devem receber / tratar os dados mais rápido do que a fonte dos dados está enviando novos dados. Em um sistema operacional de grande porte, como Linux e windows, pode-se notar que quanto maior o acesso a dispositivos de I/O (_hard disk_, por exemplo), mais o operador sente a redução de performance para outras tarefas. Isto deve-se aos serviços do sistema que gerenciam I/Os serem de alta prioridade.
  1. Alguns algoritmos sugerem que tarefas que são realizadas de forma rápida devem possuir maior prioridade, enquanto que tarefas mais lentas recebem menores prioridades. Desta forma uma tarefa lenta pode ser interrompida diversas vezes durante sua operação por tarefas de maior prioridade. O principal algoritmo desta linha de pensamento é o Rate Monotonic (RM).
  1. Tarefas que não interferem diretamente na operação do sistema embarcado, como atualizar um display, por exemplo, podem ter prioridades baixas.