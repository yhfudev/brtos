#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "types.h"


#define vFCDIV 0x4F // divisor para o FCLK = (BUSCLK) / (8 * 63+1)
                  // FCLK = 24Mhz / (128) = 187,5 Khz
                  // O valor máximo não pode ultrapassar 200 Khz
                  
                  
/* Assembly sub-routines */
//void FlashErase(unsigned char *); /* prototype for FlashErase routine */
                               /* Page Erase command */
//void FlashProg(unsigned char *, unsigned char); /* prototype for FlashProg routine */
                                             /* Byte Program command */
                  

 // No arquivo project.PRM deve-se separar o espaço de pelo menos
 // uma página para utilizar como memória não-volátil
 
 
 //Lê um endereço de memória
 unsigned char flash_read(unsigned int endereco)
 {
  unsigned char *ponteiro;
  //guarda o endereço da memória em um ponteiro
  ponteiro = (char*) endereco;
  //retorna o valor armazenado no endereço indicado pelo ponteiro
  return (*ponteiro);
 }
 
 
 
 
 //Escreve em um endereço da memória FLASH
 unsigned char flash_write(INT32U endereco, INT32U dado)
 {
  INT32U *ponteiro;
  
  //guarda o endereço da memória em um ponteiro 
  ponteiro = (INT32U*) endereco;
  
  //Verifica e apaga flag de erro de acesso a FLASH
  if(FSTAT_FACCERR)
     FSTAT_FACCERR = 1;
  
  //Verifica e apaga flag de violação de conteúdo protegido da FLASH
  if(FSTAT_FPVIOL)
     FSTAT_FPVIOL = 1;
  
  // Indica que um comando da FLASH está sendo executado
  FSTAT_FCBEF = 1;
  
  
  //Chama a função de programação da FLASH
  
  *ponteiro = dado;
  
  // Indica procedimento de escrita
  FCMD = 0x20;
  
  // Indica que um comando da FLASH está sendo executado
  FSTAT_FCBEF = 1;  
  
  //se houver erro, retorna 1. Caso contrário, retorna 0
  if(FSTAT_FACCERR || FSTAT_FPVIOL)
    return(1);
  else
    return(0);
 }
  
  
 // Apaga uma página na FLASH
 unsigned char flash_page_erase(unsigned int endereco)
 {
  unsigned char *ponteiro;
  //guarda o endereço da memória em um ponteiro
  ponteiro = (char*) endereco;
  //Verifica e apaga flag de erro de acesso a FLASH
  if(FSTAT_FACCERR)
    FSTAT_FACCERR = 1;
  //Chama a função de programação da FLASH
  
  /////////////////////////////////////////////
  // FlashErase(ponteiro);
  /////////////////////////////////////////////
  
  //se houver erro, retorna 1. Caso contrário, retorna 0
  if(FSTAT_FACCERR || FSTAT_FPVIOL)
    return(1);
  else
    return(0);
 }
  
  // Inicializa o controlador da memória FLASH
  void init_FLASH(void){
  unsigned char va=0;
    if(FSTAT_FACCERR)
      FSTAT_FACCERR = 1;
    FCDIV = vFCDIV;
  }
  
  
  
// Função para escrita de uma frase no Flash
void write_Flash(unsigned int endereco, char *string){
byte z=0;
byte testa=0;
while(*string){
  testa = flash_write((endereco+z),*string);
  z++;
  string++;
}
}