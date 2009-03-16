#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#include "myRTOS.h"
#include "drivers.h"
#include "event.h"
#include "queue.h"
#include "OS_RTC.h"
#include "MRF24J40.h"


// Declara uma estrutura de fila
extern OS_QUEUE SerialPortBuffer;

// Declara um ponteiro para o bloco de controle da Porta Serial
extern OS_EVENT *Serial;
extern OS_EVENT *Teclado;
extern OS_EVENT *RF_Event;
extern OS_EVENT *ConversorAD;


extern INT8U debug_display;


static void Task_1(void)
{
  /* task setup */
  volatile INT8U teste=0;
  OSResetTime(&Hora);
  /* task main loop */
  
  for (;;) 
  {
      OSUpdateUptime(&Hora,&Data);
      PTHD_PTHD0 = ~PTHD_PTHD0;
      //if (PTED_PTED3 == 1)
      //  teste=PHYGetShortRAMAddr(READ_ISRSTS);
      //if (teste != 0)
      //  while(1){};
      DelayTask(1000);
  }
}


static void Task_2(void)
{
   /* task setup */
  
   /* task main loop */
   for (;;)
   {
      PTHD_PTHD1 = ~PTHD_PTHD1;
      #if (COP == 1)
        __RESET_WATCHDOG();
      #endif
      DelayTask(10);
   }
}


static void Task_3(void)
{
   /* task setup */
   INT16U i=0;
  
   /* task main loop */
   for (;;) 
   {
      DelayTask(3);
      
      while(SerialPortBuffer.OSQEntries>0)
      {
        //VetorSerial[i] = OSRQueue(&SerialPortBuffer);
        acquireSCI2();
        Serial_Envia_Caracter(OSRQueue(&SerialPortBuffer));
        releaseSCI2();
        /*
        i++;
        if (i == 3)
        {
          i = 0;
          switch(VetorSerial[0])
          {
            case 49:
              acquireSCI2();
              Transmite_CPU_Load();
              releaseSCI2();
              break;
            case 50:
              acquireSCI2();
              Transmite_Uptime();
              releaseSCI2();
              break;
            case 51:
              acquireSCI2();
              Serial_Envia_Frase("myRTOS Version 0.8");
              Serial_Envia_Caracter(10);
              Serial_Envia_Caracter(13);
              releaseSCI2();
              break;
            case 52:
              acquireSCI2();
              Transmite_RAM_Ocupada();
              releaseSCI2();
              break;
            case 53:
              acquireSCI2();
              Transmite_Task_Stacks();
              releaseSCI2();
              break;
            case 54:
              acquireSCI2();
              Transmite_Duty_Cycle();            
              releaseSCI2();
              break;              
            default:
              acquireSCI2();
              Serial_Envia_Frase("Codigo Invalido");
              Serial_Envia_Caracter(10);
              Serial_Envia_Caracter(13);
              releaseSCI2();
              break;
          }
          
        }*/
      }  
   }
}


static void Task_4(void)
{
   /* task setup */   
   INT8U tecla=0;
   INT16S correcao = 0;
   volatile INT8U i=0;

  
   /* task main loop */
   for (;;)
   {
   
      // Stop Task to wait button click
      OSSemPend (Teclado,0);
      // Espera por estabilização do botão
      DelayTask(50);      
        // Módule = 1200
          
      //correcao = TPM3C1V;
      correcao = 100;
      tecla = PTDD & 0x0F;
      
      if (tecla == 0x0E)
      {
        correcao = correcao + 24;
        if (correcao > 1150)
          correcao = 1150;
        //TPM3C1V = correcao;
      }
        
      if (tecla == 0x0D)
      {
        correcao = correcao - 24;
        if (correcao < 50)
          correcao = 50;
        //TPM3C1V = correcao;
      }
      
      
      if (tecla == 0x0B)
      {
        if (debug_display < 2)
          debug_display = debug_display ^ 1;
      }
      
      if (tecla == 7)
      {
          //debug_display = 2;
          i = PHYGetShortRAMAddr(READ_RXMCR);
          acquireSCI2();
          Serial_Envia_Caracter(i);
          i = PHYGetShortRAMAddr(READ_SADRL);
          Serial_Envia_Caracter(i);
          i = PHYGetShortRAMAddr(READ_SADRH);
          Serial_Envia_Caracter(i);
          i = PHYGetShortRAMAddr(READ_ORDER);
          Serial_Envia_Caracter(i);
          i = PHYGetShortRAMAddr(READ_TXMCR);
          Serial_Envia_Caracter(i);
          i = PHYGetLongRAMAddr(RFSTATE);
          Serial_Envia_Caracter(i);          
          i = PHYGetShortRAMAddr(READ_ISRSTS);
          Serial_Envia_Caracter(i);
          i = PHYGetShortRAMAddr(READ_FFOEN);
          Serial_Envia_Caracter(i);
          i = PHYGetLongRAMAddr(RFCTRL0);
          Serial_Envia_Caracter(i);          
          
          Serial_Envia_Frase("Reset do Radio");
          releaseSCI2();
          
          //MRF24J40Init();  
      }          
      
      // Evita problemas com mais de uma interrupção de teclado seguida
      KBI2SC_KBACK = 1;
      KBI2SC_KBIE = 1;
      KBI2SC_KBACK = 1;
   }
}


/*
static void Task_5(void)
{
   // task setup
   INT16U percent = 0;
   INT8U caracter = 0;
   
   
   acquireLCD();
   posiciona(1,1);
   write_lcd("MyRTOS 0.8");
   releaseLCD();
   //posiciona(2,1);
   //write_lcd("Duty Cycle: ");
  
   // task main loop
   for (;;) 
   {
      
      //posiciona(2,13);
      
      if (debug_display == 0)
      {
        //OSEnterCritical;
        //percent = (TPM3C1V*100)/TPM3MOD;
        percent = 75;
        //OSExitCritical;    
    
        acquireLCD();
        posiciona(2,1);
        write_lcd("Duty Cycle: ");    
        caracter = (percent / 10) + 48;
        caractere(caracter);
        caracter = (percent%10) + 48;
        caractere(caracter);
        caractere('%');
        caractere(' ');
        releaseLCD();
      }

      if (debug_display == 1)
      {      
        acquireLCD();
        posiciona(2,1);
        Transmite_Uptime_Display();
        releaseLCD();
      }
      
      
      
      if(debug_display == 2)
      {
        // Initialize Display LCD
        acquireLCD();
        init_lcd();      
        posiciona(1,1);
        write_lcd("MyRTOS 0.8");
        posiciona(2,1);
        write_lcd("Duty Cycle: ");
        releaseLCD();
        debug_display = 0;
      }
      
      
      DelayTask(200);
   }
}

// Antiga tarefa 5

   //INT16U atualiza_pwm = 0;

      //DelayTask(10);   
      //atualiza_pwm = converte(2);
      //Mensagem = atualiza_pwm;
      //TPM3C0V = atualiza_pwm >> 2;
      
      // Exemplo de MailBox Post
      //OSMboxPost(ConversorAD,(void *)&Mensagem);

*/

static void Task_5(void)
{
   /* task setup */
   INT8U address=0;
  
  
   /* task main loop */
   for (;;)
   {
      address = PHYGetShortRAMAddr(READ_BBREG1);
      if (address)
      {
        MRF24J40Init();
      }
      DelayTask(1000);
   }
}


static void Task_6(void)
{
  /* task setup */
  //INT16U recebe;
  volatile INT8U recebe=0;
  volatile MRF24J40_IFREG flags;
  
  INT8U caracter = 0;
  INT8U mil,cent,dez;
  INT16U j=0;  

  extern volatile INT8U rssiWrite;
  extern volatile INT8U rssiBuffer[RSSI_BUFFER_SIZE];
  extern INT8U numRSSISamples;
  extern volatile DWORD_VAL numPacketsRx;
  extern INT16U numPackRx;
  
  /* task main loop */
  numPackRx = 0;
  
  for (;;)
  {
      // Stop Task to wait button click
      OSSemPend (RF_Event,0);
      
      // Desabilita interrupção de hardware ligada ao pino INT
      TPM3C0SC_CH0IE = 0;
      
      //create a new scope for the MRF24J40 interrupts so that we can clear the interrupt
      //flag quickly and then handle the interrupt that we have already received
        
      //read the interrupt status register to see what caused the interrupt        
      flags.Val=PHYGetShortRAMAddr(READ_ISRSTS);
           

      if(flags.bits.RF_TXIF)
      {
        //if the TX interrupt was triggered
        BYTE_VAL results;                       
                
        //read out the results of the transmission
        results.Val = PHYGetShortRAMAddr(READ_TXSR);
                
        if(results.bits.b0 == 1)
        {
          //the transmission wasn't successful and the number
          //of retries is located in bits 7-6 of TXSR
          //failed to Transmit
        }
        else
        {
          //transmission successful
          //MAC ACK received if requested else packet passed CCA
        }
      }
            
      if(flags.bits.RF_RXIF)
      {   
         volatile INT8U i,j;
         volatile INT8U RSSI_VAL;
         INT8U receive_byte=0;
         //INT16U p;
         //DWORD_VAL v;
                   
         //packet received
         //Need to read it out
                
         i=PHYGetLongRAMAddr(0x300); //first byte of the receive buffer is the packet length (this 
         numPacketsRx.Val++;
        
         // Incrementa o numero de pacotes recebidos
         numPackRx++;        

         RSSI_VAL = PHYGetLongRAMAddr(0x300+i+2);

         //PrintChar(RSSI_VAL);
         //PrintChar(i);   //print the packet length
            
         //  Disable receiving packets off air
         PHYSetShortRAMAddr(WRITE_BBREG1,0x04);
            
         for(j=1;j<=i;j++)
         {
            receive_byte = PHYGetLongRAMAddr(0x300 + j);    //read out the rest of the buffer
            OSWQueue(&SerialPortBuffer,receive_byte);
         }
                
         rssiBuffer[rssiWrite++] = RSSI_VAL;
         if(rssiWrite >= numRSSISamples)
         {
            rssiWrite = 0;
         }
        
         /////////////////////////////////////////
         /////////////////////////////////////////
         OSWQueue(&SerialPortBuffer,' ');
         
         mil = (numPackRx / 1000);
         caracter = mil + 48;
         if(caracter != 48)
            OSWQueue(&SerialPortBuffer,caracter);
        
         cent = ((numPackRx - mil*1000)/100);
         caracter = cent + 48;
         OSWQueue(&SerialPortBuffer,caracter);
        
         dez = ((numPackRx - 1000*mil - cent*100)/10);
         caracter = dez + 48;
         OSWQueue(&SerialPortBuffer,caracter);
        
         caracter = (numPackRx%10) + 48;
         OSWQueue(&SerialPortBuffer,caracter);
         
         OSWQueue(&SerialPortBuffer,' ');
         
         Transmite_Uptime_SPI();
         /////////////////////////////////////////
         /////////////////////////////////////////
        
         // Reinicia o ponteiro do buffer RX
         //PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
        
         // bypass MRF24J40 errata 5
         //PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
        
         //  Enable receiving packets off air
         PHYSetShortRAMAddr(WRITE_BBREG1,0x00);
        
      }
      
      /*
      if (flags.Val != 8){
         numPackRx = 0;
         // flush the RX fifo
         PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);    
    
        // Configure Unslotted CSMA-CA Mode
        //caracter = PHYGetShortRAMAddr(READ_TXMCR);
        //caracter = caracter & 0xDF;
        //PHYSetShortRAMAddr(WRITE_TXMCR,caracter);
      }
      */
      
      // Reinicia o ponteiro do buffer RX
      PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
      // bypass MRF24J40 errata 5
      PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
      
      
                                  
      (void)TPM3C0SC;                   // Lê o registrador TPM2C0SC
      TPM3C0SC_CH0F = 0;                // Limpa a flag CH0F do registrador TPM2C0SC
      TPM3C0SC_CH0IE = 1;               // Habilita interrupção de hardware ligada ao pino INT
      
      // Exemplo de MailBox Pend
      //recebe = *(INT16U *)OSMboxPend(ConversorAD,(INT16U)1000);
      //acquireSCI2();
      //Serial_Envia_Frase("Teste");
      //Serial_Envia_Caracter(10);
      //Serial_Envia_Caracter(13);
      //releaseSCI2();
      //DelayTask(150);
      //recebe = spectral_energy(11);
      //if(recebe > 120)
      //{
      //  recebe = 0;
      //}
      //DelayTask(100);
  }
}



