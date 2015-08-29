**BRTOS - Brazilian Real-Time Operating System** 

&lt;BR&gt;

 Acrônimo: Basic Real-Time Operating System

Please see English version below.

Sistema operacional de tempo real para microcontroladores de pequeno porte.

**Escalonamento:** Preemptivo por prioridades. Cada tarefa deverá ser associada a uma prioridade. Número máximo de tarefas instaladas = 32.
**Recursos de gerenciamento:** Semáforos, mutex, caixas de mensagens e filas. O mutex utiliza o protocolo _priority ceiling_ com o intuito de evitar _deadlocks_ e inversões de prioridade.

O sistema é escrito em linguagem C, possuindo algumas chamadas em assembly no HAL (_Hardware Abstraction Layer_).

**Ports oficiais:** Freescale Kinetis (ARM Cortex-M4), Freescale Coldfire V1, Freescale HCS08,  ST STM32F4xx (ARM Cortex-M4F), NXP LPC11xx (ARM Cortex-M0), NXP LPC176x (ARM Cortex-M3), Renesas RX600 (RX62N), Texas Instruments MSP430, Texas Instruments Stellaris LM3S8968 (ARM Cortex-M3), Texas Instruments Stellaris LM4F120H5QR (ARM Cortex-M4F), Atmel ATMEGA328/128 e Microchip PIC18. 

&lt;BR&gt;



**Requisitos mínimos:**
O Sistema Operacional ocupa menos de 100 bytes de RAM e 2KB de memória de programa com seus recursos mínimos. Pode chegar a 1KB de RAM e 8KB de memória de programa caso sejam utilizados todos os serviços do sistema e o número máximo de tarefas (32).

**Maiores detalhes sobre o BRTOS estão disponíveis no Wiki do projeto e no [Blog do BRTOS](http://brtosblog.wordpress.com).**


&lt;BR&gt;




&lt;BR&gt;


**English version**

BRTOS is a lightweight preemptive real time operating system designed for low end microcontrollers.

**Scheduler:** priority-based preemptive scheduler. A priority must be assigned for each task (aka thread). Max. number of installed tasks = 32.

**Resources:** Semaphores, mutexes, message queues, mailboxes. Mutex makes use of a _priority ceiling_ protocol in order to avoid _deadlocks_ and unbounded priority inversion.

BRTOS kernel is written mostly in C language, with little assembly code in the HAL file(_Hardware Abstraction Layer_).

**Official ports:** Freescale Kinetis (ARM Cortex-M4), Freescale Coldfire V1, Freescale HCS08,  ST STM32F4xx (ARM Cortex-M4F), NXP LPC11xx (ARM Cortex-M0), NXP LPC176x (ARM Cortex-M3), Renesas RX600 (RX62N), Texas Instruments MSP430, Texas Instruments Stellaris LM3S8968 (ARM Cortex-M3), Texas Instruments Stellaris LM4F120H5QR (ARM Cortex-M4F), Atmel ATMEGA328/128 and Microchip PIC18. 

&lt;BR&gt;



**Minimum requirements:**
BRTOS kernel uses less than 100 bytes of RAM and 2KB of program memory with minimal resources. It can use up to 1KB of RAM and 8KB program memory if all system services are used and all 32 tasks are installed.



&lt;BR&gt;




&lt;BR&gt;



<a href='http://www.youtube.com/watch?feature=player_embedded&v=JPyDWiztaiQ' target='_blank'><img src='http://img.youtube.com/vi/JPyDWiztaiQ/0.jpg' width='425' height=344 /></a> <a href='http://www.youtube.com/watch?feature=player_embedded&v=IdPhrUW5iXM' target='_blank'><img src='http://img.youtube.com/vi/IdPhrUW5iXM/0.jpg' width='425' height=344 /></a>

<a href='http://www.youtube.com/watch?feature=player_embedded&v=2K_l2BTTTOY' target='_blank'><img src='http://img.youtube.com/vi/2K_l2BTTTOY/0.jpg' width='425' height=344 /></a> <a href='http://www.youtube.com/watch?feature=player_embedded&v=EOIwM1TDcc8' target='_blank'><img src='http://img.youtube.com/vi/EOIwM1TDcc8/0.jpg' width='425' height=344 /></a>