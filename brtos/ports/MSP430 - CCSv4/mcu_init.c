#include "BRTOS/includes/BRTOS.h"


//void __low_level_init(void);

void mcu_init(void) {
  
  volatile INT16U i;
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog
  
  if (CALBC1_16MHZ ==0xFF || CALDCO_16MHZ == 0xFF)                                     
  {  
    for(;;);                               // If calibration constants erased
                                            // do not load, trap CPU!!
  } 
  
  BCSCTL1 = CALBC1_16MHZ;                    // Load DCO constants
  DCOCTL = CALDCO_16MHZ;
  
  for (i=0xFFFF; i>0; i--);
  
  // init ports  
  
  P1DIR |= 0x03;                             // P1.0 outputs
  
}

/*
void __low_level_init(){

	WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog

}
*/

