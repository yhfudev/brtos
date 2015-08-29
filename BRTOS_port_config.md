# 1. Dicas de configuração / port do BRTOS #

Este artigo faz parte da documentação oficial do BRTOS e explicará configurações básicas do BRTOS e apresentará dicas de como portá-lo para outras arquiteturas.

Note que em alguns pontos será diferenciado a versão 1.1 (e superiores) do BRTOS. O escalonador do sistema foi modificado nesta versão. Estas modificações reduziram a latência das interrupções e aceleraram o escalonador. Em um conjunto de tarefas teste, a ocupação de CPU baixou de 2,65% para 1,29%.

**Importante**:
Ao usar interrupções é importante que o código de usuário seja executado através de uma chamada de função, para evitar que o compilador desalinhe a pilha.

```

void Toggle(void){
  PTAD_PTAD1 = ~PTAD_PTAD1;
  PTAD_PTAD2 = ~PTAD_PTAD2;
}


#pragma TRAP_PROC
void FastTick(void)
{
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();

  // Tratamento da interrupção 
  TIMER_HANDLER;
    
  // ***************************************
  // Habilita aninhamento de interrupções
  // ***************************************
  OS_ENABLE_NESTING();
 
  // aqui vai a chamada de função do código de usuário declarada acima
  Toggle();

  // ************************
  // Saída de interrupção
  // ************************
  OS_INT_EXIT();
}
```



As dicas / configurações serão organizadas pelos arquivos do BRTOS.



&lt;BR&gt;


## 1.1 hardware.h ##

O arquivo **hardware.h** contêm os includes de arquivos da arquitetura / MCU utilizado. Estes includes normalmente informam o endereço dos registradores do MCU, os códigos para habilitar / desabilitar interrupções, modos de baixo consumo, entre outros.

Exemplo de hardware.h do Coldfire V1 e HCS08:
```
#include <hidef.h>               /* for EnableInterrupts macro */
#include "derivative.h"          /* include peripheral declarations */
```




&lt;BR&gt;


## 1.2 BRTOS.h ##

O arquivo **BRTOS.h** contêm parâmetros do sistema, definições relativas a memória, clock e muitas outras configurações. Este arquivo apresentará a seguinte estrutura:

```
// Define if simulation or DEBUG
#define DEBUG 1

// Define if whatchdog active
#define WATCHDOG 1

/// Define Number of Priorities
#define NUMBER_OF_PRIORITIES 32

// Define CPU Stack Pointer Size
#define SP_SIZE 32

// Define the number of Task to be Installed
// must always be equal or higher to NumberOfInstalledTasks
#define NUMBER_OF_TASKS 6

// Define if OS Trace is active
#define OSTRACE 1

#if (OSTRACE == 1)  
  #include "debug_stack.h"
#endif

/// Define if TimerHook function is active
#define TIMER_HOOK_EN 1


/// TickTimer Defines
#define configCPU_CLOCK_HZ          (INT32U)25165824    ///< CPU clock in Hertz
#define configTICK_RATE_HZ          (INT32U)1000        ///< Tick timer rate in Hertz
#define configTIMER_PRE_SCALER      0                   ///< Informs if there is a timer prescaler
#define TickCountOverFlow           (INT16U)32000       ///< Determines the tick timer overflow


/// Stack Defines
/// Coldfire with 8KB of RAM: 40 * 128 bytes = 5KB of Virtual Stack
#define HEAP_SIZE 48*128


// Others Defines
#define configMAX_TASK_NAME_LEN 16
#define configMAX_TASK_INSTALL 32
```

  * **DEBUG** é utilizado em algumas plataformas que possuem diferenciações entre o simulador e o debugger. Desta forma o código se adapta a condição desejada. **DEBUG = 1** indica modo debugger / gravação do código no microcontrolador.

  * **WATCHDOG** indica se o _watchdog_ do sistema estará ativo. **WATCHDOG = 1** indica _watchdog_ ativo.

  * **NUMBER`_`OF`_`PRIORITIES** só é válido para a versão 1.1 do BRTOS, que mudou seu escalonador. É utilizado para informar ao sistema a quantidade de prioridades disponíveis. Os valores válidos são 16 ou 32. Recomenda-se utilizar no máximo 16 prioridades para microcontroladores de 8 bits e 32 prioridades para microcontroladores de 16 e 32 bits.

  * **SP`_`SIZE** é utilizado para informar ao compilador o tamanho do registrador _stack pointer_ do processador em bits. Os valores válidos para _Stack Pointer_ são 16 e 32. Por exemplo, o microcontrolador HCS08 utiliza **SP`_`SIZE = 16**. Já o microcontrolador Coldfire V1 utiliza **SP`_`SIZE = 32**.

  * **NUMBER`_`OF`_`TASKS** indica quantas tarefas podem ser instaladas em uma aplicação. O valor máximo para este _define_ é 31. Esta definição permite que uma menor quantidade de memória seja alocada para estrutura de contexto de tarefas. Utilize este recurso para reduzir o consumo de memória do sistema quando um número pequeno de tarefas for instalado.

  * **OS`_`TRACE** habilita ou desabilita o _trace_ do sistema. Com este recurso habilitado é possível monitorar o comportamento das tarefas, interrupções e chamadas de sistema. Desta forma torna-se possível identificar comportamentos errôneos, chamadas indevidas, entre outros problemas que possam vir a ocorrer em uma determinada aplicação.

  * **TIMER`_`HOOK`_`EN** habilita ou desabilita uma função ancora no Tick do sistema. Esta função pode ser utilizada para implementar trechos curtos de código que dependam de um temporizador. Como estes trechos de código são curtos, não justificam a utilização de uma tarefa. CUIDADO: Se o sistema não estiver permitindo a utilização de aninhamento de interrupções, esta função estará em região com interrupções bloqueadas.

  * **configCPU`_`CLOCK`_`HZ** indica a freqüência de barramento utilizada pelo microcontrolador em hertz.

  * **configTICK`_`RATE`_`HZ** define o _Timer Tick_ (marca de tempo) do sistema, ou seja, a resolução do gerenciador de tempo do RTOS. Valores entre 1ms (1000 Hz) e 10ms (100 Hz) são recomendados. Nunca esqueça que a resolução do gerenciamento de tempo é de + ou - 1 _Timer Tick_.

  * **configTIMER`_`PRE`_`SCALER** pode ser utilizado no _port_ do sistema para a configuração do hardware responsável pelo _Timer Tick_.

  * **TickCountOverFlow** determina o número máximo de marcas de tempo do sistema que podem ser utilizadas pelo gerenciador de tempo. Imagine o caso onde se utiliza um _Timer Tick_ de 1ms. Para gerar um _delay_ maior do que 32000ms pode ser necessário utilizar duas chamadas da função **DelayTask** ou utilizar a função **DelayTaskHMSM** que opera em milisegundos, segundos, minutos e horas.

  * **HEAP`_`SIZE** determina a quantidade de memória alocada como pilha virtual das tarefas. Sempre que uma tarefa é instalada, a quantidade de memória utilizada pela tarefa será alocada no _HEAP_.

  * **configMAX`_`TASK`_`NAME`_`LEN** configura o tamanho máximo em caracteres para o nome da tarefa. Por padrão do sistema, a associação de nomes às tarefas é desativada, devido ao consumo excessivo de memória.

  * **configMAX`_`TASK`_`INSTALL**
**Versão inferiores a 1.1** - determina o número máximo de tarefas que o escalonador do sistema pode gerenciar. O valor máximo deste _define_ é 32. Este valor indica ainda a posição de instalação da tarefa de espera (_Idle Task_). Quando se utiliza poucas tarefas, recomenda-se diminuir o valor deste _define_. Por exemplo, em uma aplicação com 6 tarefas, reduzir este valor p/ 7 limita a associação de prioridades disponíveis para as tarefas. Neste caso, as prioridades disponíveis seriam de 0 a 6, onde a prioridade 6 é ocupada pela _Idle Task_.
**OBS: Quanto menor o valor de prioridade associado a uma tarefa, maior sua prioridade no sistema.**

**Versão 1.1** - determina o número máximo de tarefas que o escalonador do sistema pode gerenciar. O valor deste _define_ é configurado automaticamente a partir da definição do número de prioridades disponíveis. As prioridades disponíveis vão de 0 a 15 ou de 0 a 31. A prioridade 0 é ocupada pela _Idle Task_.
**OBS: Quanto maior o valor de prioridade associado a uma tarefa, maior sua prioridade no sistema.**




&lt;BR&gt;


## 1.3 HAL.h ##

O arquivo **HAL.h** contêm as definições da camada de abstração de hardware do sistema e a definição do microcontrolador utilizado.

**Definição do microcontrolador:**
```
/// Define if nesting interrupt is active or not
#define NESTING_INT 0

// Define if Coldfire
#define Coldfire 1              ///< Enables or disables the coldfire core as the default processor
#define HCS08 0                 ///< Enables or disables the HCS08 core as the default processor
```

  * **NESTING`_`INT** habilita ou desabilita o aninhamento de interrupções no sistema.


**Salvar e restaurar contexto. Salvar stack pointer.**

**Coldfire V1:**
```
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Context Define                         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void SaveContext(void)
* \brief Save context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void SaveContext(void) 
{  
	LEA	 -40(A7),A7				// reserve space on current stack
	MOVEM.L  D3-D7/A2-A6,(A7)			// save CPU registers
}

/// Save Context Define
#define OS_SAVE_CONTEXT() SaveContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Save Stack Pointer Define                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void SaveCurrentSP(void)
* \brief Save current stack pointer function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void SaveCurrentSP(void) 
{  
	MOVE     A7,SPvalue           // save top of stack
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Restore Context Define                      /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*****************************************************************************************//**
* \fn inline asm void RestoreContext(void)
* \brief Restore context function
* \return NONE
*********************************************************************************************/
inline asm __declspec(register_abi) void RestoreContext(void)
{  
	MOVE    SPvalue,A7              // restore top of stack
	MOVEM.L (A7),D3-D7/A2-A6  	// restore other CPU registers
	LEA     40(A7),A7		// adjust stack pointer value
}

/// Restore Context Define
#define OS_RESTORE_CONTEXT() RestoreContext()
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
```

**HCS08:**
```
#define SaveCurrentSP() \
{                       \
  asm("TSX");           \
  asm("STHX SPvalue");  \
}



#define OS_RESTORE_CONTEXT() \
{                            \
  asm("LDHX SPvalue");       \
  asm("TXS");                \
}
```

**OBS: O único contexto a ser salvo no HCS08 é o registrador H. Como o compilador utilizado inclui automaticamente o código para salvar este registrador nas interrupções, o port não precisa implementar o salvamento de contexto.**



&lt;BR&gt;


**Outras definições do hardware:**

Ex: Coldfire V1
```
/// Defines the change context command of the choosen processor
#define ChangeContext(void) asm ("TRAP %14");
/// Defines the disable interrupts command of the choosen microcontroller
#define OSEnterCritical DisableInterrupts;
/// Defines the enable interrupts command of the choosen microcontroller
#define OSExitCritical EnableInterrupts;
/// Defines the low power command of the choosen microcontroller
#define OS_Wait asm(STOP #0x2000);
/// Defines the tick timer interrupt handler code (clear flag) of the choosen microcontroller
#define TICKTIMER_INT_HANDLER TPM1SC_TOF = 0
#define TIMER_MODULE TPM1MOD


// stacked by the RTI interrupt process
// Mínimo de 60 devido ao salvamento de 15 registradores de 32 bits da CPU
// D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes)
/// Minimum size of a task stack.
/// Example in the coldfire processor: D0-D7 (32 bytes) + A0-A6 (28 bytes) + Format (1 byte) + Vector (1 byte) + SR (2 bytes) + SP (4 bytes) = 68 bytes
#define NUMBER_MIN_OF_STACKED_BYTES 68
```

Ex: HCS08
```
#define ChangeContext() asm ("SWI");
#define OSEnterCritical asm ("SEI");
#define OSExitCritical asm ("CLI");
/// Defines the tick timer interrupt handler code (clear flag) of the choosen microcontroller
#define TICKTIMER_INT_HANDLER TPM1SC_TOF = 0
#define TIMER_MODULE TPM1MOD
#define TIMER_COUNTER TPM1CNT

//Stack Defines
#define NUMBER_MIN_OF_STACKED_BYTES 6
```

  * **ChangeContext(void)** Define o código em assembly para chamada de interrupção de software utilizada pela troca de contexto.

  * **OSEnterCritical** Define a função que coloca o código do BRTOS em região crítica, ou seja, desabilita as interrupções.

  * **OSExitCritical** Define a função que retira o código do BRTOS da região crítica, ou seja, habilita as interrupções.

  * **OS\_Wait** Define a rotina de código que coloca o microcontrolador em modo de baixo consumo na Idle Task.

  * **TICKTIMER\_INT\_HANDLER** Define a rotina de código que limpa a flag da interrupção utilizada para gerar o Tick timer do sistema.

  * **TIMER\_MODULE** Define o registrador de módulo do temporizador utilizado pelo Tick Timer do sistema. Esta informação é utilizada para o cálculo do tempo de ocupação da CPU.

  * **NUMBER\_MIN\_OF\_STACKED\_BYTES** Define o número de bytes ocupado para guardar um contexto do sistema.




&lt;BR&gt;


## 1.4 HAL.c ##

O arquivo **HAL.c** contêm as funções da camada de abstração de hardware do sistema.

A função _TickTimerSetup_ deve ser utilizada para implementar a rotina de configuração do Tick Timer do sistema. Valores típicos entre 1ms e 10ms.

```
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      OS Tick Timer Setup                         /////
/////                                                  /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void TickTimerSetup(void)
{
  
  /* ### Init_TPM init code */
  /* TPM1SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  TPM1SC = 0x00;                       /* Stop and reset counter */
  TPM1MOD = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) >> configTIMER_PRE_SCALER; /* Period value setting */
  (void)(TPM1SC == 0);                 /* Overflow int. flag clearing (first part) */
  /* TPM1SC: TOF=0,TOIE=1,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
  TPM1SC = 0x48 | configTIMER_PRE_SCALER;                       /* Int. flag clearing (2nd part) and timer control register setting */
  
  /* ### */
}
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
```

A função **TickTimerSetup** deve ser utilizada para implementar a rotina de configuração do Tick Timer do sistema. Valores típicos entre 1ms e 10ms.


As funções **InstallTask** e **InstallIdle** são praticamente independentes do microcontrolador. O que faz com que estejam incluídas no HAL é a preparação do stack para o boot inicial da tarefa.

O protótipo destas função é apresentado abaixo:
```
INT8U InstallTask(void(*FctPtr)(void),const char *TaskName, INT16U USER_STACKED_BYTES,INT8U iPriority)

INT8U InstallIdle(void(*FctPtr)(void), INT16U USER_STACKED_BYTES)
```



Para exemplificar a diferença de um port destas funções será apresentado o trecho de instalação que precisa ser portado para o HCS08 e para o Coldfire V1.

**HCS08:**
```
   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned long) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned long) (FctPtr)) >> 8;   
```



&lt;BR&gt;



**Coldfire V1:**
```
   // First 4 bytes defined to Coldfire Only
   // Format: First 4 bits = processor indicating a two-longword frame, always 0x04 in MCF51QE
   //         Other 4 bits = fault status field, always 0x00 if no error occurred
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 8] = 0x40;
   // Vector
   // The 8-bit vector number, vector[7:0], defines the exception type and is
   // calculated by the processor for all internal faults and represents the
   // value supplied by the interrupt controller in the case of an interrupt
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 7] = 0x80;
   
   // Initial SR Register
   // Interrupts Enabled
   // CCR = 0x00
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 6] = 0x20;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 5] = 0x00;
   
   // Pointer to Task Entry
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 1] = ((unsigned long) (FctPtr)) & 0x00FF;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 2] = ((unsigned long) (FctPtr)) >> 8;   
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 13] = 0xA1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 17] = 0xA0;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 21] = 0xD2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 25] = 0xD1;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 29] = 0xD0;
   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 33] = 0xA6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 37] = 0xA5;   
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 41] = 0xA4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 45] = 0xA3;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 49] = 0xA2;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 53] = 0xD7;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 57] = 0xD6;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 61] = 0xD5;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 65] = 0xD4;
   STACK[iStackAddress + NUMBER_OF_STACKED_BYTES - 69] = 0xD3;   
```



&lt;BR&gt;


Note que o microcontrolador HCS08 utiliza uma arquitetura CISC com poucos registradores. Como a estrutura HEAP é inicializada com zeros, não é necessário inicializar o contexto dos registrador A e X com zero, somente definir o ponteiro para a entrada da tarefa.

Já o Coldfire V1, possui muitos 7 registradores de endereço e 7 registradores de dados. Ainda, é necessário definir a condição inicial do registrador de condição e do vetor de controle do sistema. Não conseguimos impedir que o compilador da Freescale salvasse uma parcela do contexto automaticamente. Desta forma, as interrupções salvam automaticamente os registradores D0,D1,D2,A0 e A1. A função para salvar o contexto do BRTOS salva o resto. Isto pode ser notado na ordem de entrada dos registradores na instalação da tarefa.



&lt;BR&gt;


**Nota:** A tarefa 1 é utilizada para gerenciar o **WATCHDOG** e o relógio do sistema. Esta tarefa é chamada a cada 10 ticks do sistema (10ms nos códigos exemplo). O **WATCHDOG** esta configurado para 32ms no exemplo. Se o sistema deixar de entrar nesta tarefa é porque está inconsistente e será resetado pelo **WATCHDOG**.

```
void Task_1(void)
{
   // task setup
   INT16U i = 0;
   INT8U  j = 0;
   OSResetTime(&Hora);
  
   // task main loop
   for (;;)
   {
      #if (WATCHDOG == 1)
        __RESET_WATCHDOG();
      #endif
      
      DelayTask(10);
      i++;
      if (i >= 100)
      {
        OSUpdateUptime(&Hora,&Data);
        i = 0;
      }
   }
}
```



&lt;BR&gt;


Era isso pessoal ... As informações necessárias para configurar e portar o BRTOS estão neste documento. Estamos a disposição para qualquer dúvida.

**OBS: O port do sistema com suporte a interrupções aninhadas exige um pouco mais de esforço. Em um próximo post iremos abordar as modificações necessárias.**