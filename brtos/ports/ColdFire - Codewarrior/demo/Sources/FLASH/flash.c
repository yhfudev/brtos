#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS_types.h"


#define vFCDIV 0x4F // divisor para o FCLK = (BUSCLK) / (8 * 63+1)
                  // FCLK = 24Mhz / (128) = 187,5 Khz
                  // O valor m�ximo n�o pode ultrapassar 200 Khz
                  
                  
/* Assembly sub-routines */
//void FlashErase(unsigned char *); /* prototype for FlashErase routine */
                               /* Page Erase command */
//void FlashProg(unsigned char *, unsigned char); /* prototype for FlashProg routine */
                                             /* Byte Program command */
                  

 // No arquivo project.PRM deve-se separar o espa�o de pelo menos
 // uma p�gina para utilizar como mem�ria n�o-vol�til
 
 
 //L� um endere�o de mem�ria
 unsigned char flash_read(unsigned int endereco)
 {
  unsigned char *ponteiro;
  //guarda o endere�o da mem�ria em um ponteiro
  ponteiro = (char*) endereco;
  //retorna o valor armazenado no endere�o indicado pelo ponteiro
  return (*ponteiro);
 }
 
 
 
 
 //Escreve em um endere�o da mem�ria FLASH
 unsigned char flash_write(INT32U endereco, INT32U dado)
 {
  INT32U *ponteiro;
  
  //guarda o endere�o da mem�ria em um ponteiro 
  ponteiro = (INT32U*) endereco;
  
  //Verifica e apaga flag de erro de acesso a FLASH
  if(FSTAT_FACCERR)
     FSTAT_FACCERR = 1;
  
  //Verifica e apaga flag de viola��o de conte�do protegido da FLASH
  if(FSTAT_FPVIOL)
     FSTAT_FPVIOL = 1;
  
  // Indica que um comando da FLASH est� sendo executado
  FSTAT_FCBEF = 1;
  
  
  //Chama a fun��o de programa��o da FLASH
  
  *ponteiro = dado;
  
  // Indica procedimento de escrita
  FCMD = 0x20;
  
  // Indica que um comando da FLASH est� sendo executado
  FSTAT_FCBEF = 1;  
  
  //se houver erro, retorna 1. Caso contr�rio, retorna 0
  if(FSTAT_FACCERR || FSTAT_FPVIOL)
    return(1);
  else
    return(0);
 }
  
  
 // Apaga uma p�gina na FLASH
 unsigned char flash_page_erase(unsigned int endereco)
 {
  unsigned char *ponteiro;
  //guarda o endere�o da mem�ria em um ponteiro
  ponteiro = (char*) endereco;
  //Verifica e apaga flag de erro de acesso a FLASH
  if(FSTAT_FACCERR)
    FSTAT_FACCERR = 1;
  //Chama a fun��o de programa��o da FLASH
  
  /////////////////////////////////////////////
  // FlashErase(ponteiro);
  /////////////////////////////////////////////
  
  //se houver erro, retorna 1. Caso contr�rio, retorna 0
  if(FSTAT_FACCERR || FSTAT_FPVIOL)
    return(1);
  else
    return(0);
 }
  
  // Inicializa o controlador da mem�ria FLASH
  void init_FLASH(void){
  unsigned char va=0;
    if(FSTAT_FACCERR)
      FSTAT_FACCERR = 1;
    FCDIV = vFCDIV;
  }
  
  
  
// Fun��o para escrita de uma frase no Flash
void write_Flash(unsigned int endereco, char *string){
byte z=0;
byte testa=0;
while(*string){
  testa = flash_write((endereco+z),*string);
  z++;
  string++;
}
}