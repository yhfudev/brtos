#define FLASH_ACCESS_ERROR    (INT8U)1
#define FLASH_VIOLATION_ERROR (INT8U)2

INT32U flash_read(INT32U address);
INT8U flash_write(INT32U address, INT32U data);
INT8U flash_page_erase(INT32U address);
void init_FLASH(void);
void write_Flash(INT32U address, INT8U *string);

