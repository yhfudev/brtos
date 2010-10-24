#include "hardware.h"
#include "OS_types.h"
#include "BRTOS.h"
#include "flash.h"


#define vFCDIV 0x4F // divisor para o FCLK = (BUSCLK) / (8 * 15+1)
                  // FCLK = 24Mhz / (128) = 187,5 Khz
                  // O valor máximo não pode ultrapassar 200 Khz
                  
INT8U big_buffer[32];

#define near
                  

                  

 // No arquivo project.lcf deve-se separar o espaço de pelo menos
 // uma página (1KB) para utilizar como memória não-volátil
 
 
 //Lê um endereço de memória
 INT32U flash_read(INT32U address)
 {
  INT32U *pointer;
  //guarda o endereço da memória em um ponteiro
  pointer = (INT32U*) address;
  //retorna o valor armazenado no endereço indicado pelo ponteiro
  return (*pointer);
 }
 
 
static flash_command_ram(INT8U cmd, INT32U *address, INT32U data)
{
      
    *address = data;

    // write the command
    FCMD = cmd;

    // Indica que um comando da FLASH está sendo executado
    FSTAT_FCBEF = 1;

    // busy wait for flash command complete
    while (!FSTAT_FCCF)
    {
    };  
}

static void flash_command(INT8U cmd, INT32U *addr, INT32U data)
{
    void *fn;
    
    FSTAT_FPVIOL = 1;
    FSTAT_FACCERR = 1;
    
    fn = (void *)(((INT32U)big_buffer+3)&~3);
    ((void (* near)(INT8U, INT32U *, INT32U))fn)(cmd, addr, data);
}
 
 
 //Escreve em um endereço da memória FLASH
 INT8U flash_write(INT32U address, INT32U data)
 {
  INT32U *pointer;

  if(currentTask)
    UserEnterCritical();  
  
  //guarda o endereço da memória em um ponteiro 
  pointer = (INT32U*) address;
 
  //Chama a função de programação da FLASH  
  *pointer = data;
  
  flash_command(0x20,pointer,data);  

  asm
  {
    NOP
    NOP
    NOP        
  }  

  if(currentTask)
    UserExitCritical();
  
  if(FSTAT_FACCERR)
    return FLASH_ACCESS_ERROR;
  
  if(FSTAT_FPVIOL)
    return FLASH_VIOLATION_ERROR;
  
  return OK;
 }
  




  
 // Apaga uma página na FLASH
INT8U flash_page_erase(INT32U address)
{
  INT32U *pointer;

  if(currentTask)
    UserEnterCritical();
    
  //guarda o endereço da memória em um ponteiro
  pointer = (INT32U*) address;
  
  flash_command(0x40,pointer,0);  
  
  asm
  {
    NOP
    NOP
    NOP        
  }
  
  if(currentTask)
    UserExitCritical();
  
  if(FSTAT_FACCERR)
    return FLASH_ACCESS_ERROR;
  
  if(FSTAT_FPVIOL)
    return FLASH_VIOLATION_ERROR;
  
  return OK;
}
 
 


  

// Inicializa o controlador da memória FLASH
void init_FLASH(void)
{
  void *fn;
  
  FSTAT = 0x30;
  
  FCDIV = vFCDIV;
  
  fn = (void *)(((INT32U)big_buffer+3)&~3);
  memcpy(fn, flash_command_ram, (INT32U)flash_command-(INT32U)flash_command_ram);  
}

  
  
// Função para escrita de uma frase no Flash
void write_Flash(INT32U address, INT8U *string)
{
  INT32U z=0;
  INT8U count=0;
  INT32U char32=0;

  while(*string)
  {
    if (count == 0)
      char32 = char32 | (INT32U)*string<<24;
  
    if (count == 1)
      char32 = char32 | (INT32U)*string<<16;
  
    if (count == 2)
      char32 = char32 | (INT32U)*string<<8;
  
    if (count == 3)
    { 
      char32 = char32 | (INT32U)*string;
      (void)flash_write((address+z),char32);
      char32=0;      
      z+=4;
    }
    string++;
    count++;
    if(count==4)
      count = 0;
  }
  if (count != 0)
  {
    (void)flash_write((address+z),char32);
  }
}