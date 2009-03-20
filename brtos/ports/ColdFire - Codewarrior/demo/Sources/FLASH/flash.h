//Protótipos de funções de acesso a memória FLASH
void init_FLASH(void);
unsigned char flash_read(unsigned int endereco);
unsigned char flash_write(unsigned int endereco, unsigned char dado);
void write_Flash(unsigned int endereco, char *string);
unsigned char flash_page_erase(unsigned int endereco);                  
