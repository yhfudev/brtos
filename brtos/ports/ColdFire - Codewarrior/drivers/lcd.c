#include "derivative.h" /* include peripheral declarations */
#include "event.h"
#include "BRTOS.h"

// Definições para escolha dos pinos da porta do microcontrolador
#define RS PTED_PTED4
#define RW PTED_PTED5
#define E PTED_PTED6
#define controle PTED
#define controle_dir PTEDD
#define dados PTFD
#define direcao PTFDD
#define busy_flag PTFD_PTFD7

// Declara um ponteiro para o bloco de controle do Display LCD
BRTOS_Mutex *LCD;

INT8U task_LCD = 0;



// Declaração do protótipo da função de teste da busy flag
void testa_bit(void);


//Função para adquirir direito exclusivo ao LCD
// Assim que possível colocar recurso de timeout
void acquireLCD(void)
{
  // Aloca o recurso do display LCD
  OSMutexPend (LCD, 0);
}

//Função para liberar o display LCD
INT8U releaseLCD(void)
{
  // Libera o recurso do display LCD
  OSMutexPend (LCD);    
}

// Função para escrita de uma frase no display
void write_lcd(char *string){

  while(*string)
  { 
    testa_bit();
    RS = 1;
    RW = 0;
    dados = *string;
    E = 1;
    asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
    }
    E = 0;
    string++;
  }
  
}


// Função para escrita de um número de 2 digitos no display
void write_numero(unsigned char numero)
{
unsigned char i = 0;
 
  testa_bit();
  RS = 1;
  RW = 0;
  i = (numero / 10);
  dados = 48 + i;
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
  }
  E = 0;
  
  testa_bit();
  RS = 1;
  RW = 0;
  dados = 48 + (numero - (i * 10));
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
  }
  E = 0;   
}


// Envia um comando para o módulo do display
void instr(char comando){
  testa_bit();
  RS = 0;
  RW = 0;
  dados = comando;
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
  } 
  E = 0;
}


// Envia um caractere para o módulo do display
void caractere(char dado)
{
  
  testa_bit();
  RS = 1;
  RW = 0;
  dados = dado;
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
  }
  E = 0;
   
}


// Inicialização do display para 2 linhas com caracteres de 5x7
// Deslocamento do cursor a direita com cursor apagado
void init_lcd(void){
  dados = 0;
  direcao = 0xFF;
  controle = 0;
  controle_dir = 0b01110000;
  instr(0x38);
  instr(0x38);
  instr(0x06);
  instr(0x0c);
  instr(0x01);  
}


  // Cria um semaforo com contador = 1, informando que o recurso está disponível
  // após a inicialização
void init_lcd_resource(INT8U priority)
{  
  // Cria um mutex com contador = 1, informando que o recurso está disponível
  // após a inicialização
  // Prioridade máxima a acessar o recurso = priority
  if (OSMutexCreate(&LCD,priority) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };
}


// Função de posicionamento do cursor do display
void posiciona(byte linha, byte coluna)
{
  
  testa_bit();
  RS = 0;
  RW = 0;
  switch(linha){
    case 1:
      coluna--;
      dados = 0x80 + coluna;
      break;
    case 2:
      coluna--;
      dados = 0xC0 + coluna;
      break;
    default:
      return;
  }
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
  }  
  E = 0;
  
}


// Função para limpar o display e retornar a primeira posição da primeira linha
void limpa_lcd(void)
{
  
  testa_bit();
  RS = 0;
  RW = 0;
  dados = 0x01;
  E = 1;
  asm{
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop
      nop              
}
  E = 0;
  
}


// Função de teste da busy flag
void testa_bit(void){
  RS = 0;
  RW = 1;
  direcao = 0x00;
  E = 1;
  asm
  {
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
  }
  while(busy_flag){};
  asm
  {
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
  }  
  E = 0;
  RW = 0;
  direcao = 0xFF;
}