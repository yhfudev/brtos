#include "derivative.h" /* include peripheral declarations */
#include <hidef.h> /* for EnableInterrupts macro */
#include "BRTOS.h"
#include "drivers.h"
#include "event.h"

#include "MRF24J40.h"
#include "SPI.h"

#include "OS_RTC.h"
#include "queue.h"
// Declara uma estrutura de fila
extern OS_QUEUE SerialPortBuffer;


//#include <stdio.h>
//#include <string.h>

//IEEE EUI - globally unique number
#define EUI_7 0x00
#define EUI_6 0x01
#define EUI_5 0x02
#define EUI_4 0x03
#define EUI_3 0x04
#define EUI_2 0x05
#define EUI_1 0x06
#define EUI_0 0x07

const INT8U testShortRegisters [] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x1C,0x1D,0x1E,0x1F,0x21,0x22,0x23,0x26,0x27,0x28,0x29,0x2E,0x34,0x3D,0x3F};
const INT16U testLongRegisters [] = {0x0222,0x0223,0x0224,0x0225,0x0226,0x0227,0x0228,0x022A,0x022B,0x022C,0x0230,0x0231,0x0232,0x0233,0x0234,0x0235,0x0236,0x0237,0x0238,0x0239,0x0240,0x0241,0x0242,0x0243,0x0244,0x0245,0x0246,0x0247,0x0248,0x0249,0x024A,0x024B,0x024C};

  
// Transceiver Configuration
// Estou usando a captura de tempo como interrupção externa
#define RFIF            TPM3C0SC_CH0F // Flag da interrupção externa
#define RFIE            TPM3C0SC_CH0IE // habilita / desabilita interrupção externa

#define PHY_CS          PTED_PTED3
#define PHY_CS_TRIS     PTEDD_PTEDD3
#define PHY_RESETn      PTCD_PTCD2
#define PHY_RESETn_TRIS PTCDD_PTCDD2
#define PHY_WAKE        PTCD_PTCD1
#define PHY_WAKE_TRIS   PTCDD_PTCDD1

#define PA_LEVEL 0x00  // -0.00 dBm
#define FREQUENCY_BAND 2400



//INT8U toReturn;


//#pragma romdata longAddressLocation = 0x0E
const INT8U myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};
//#pragma romdata


INT8U channel;
INT8U power;
INT8U mode;
INT8U UseExternal;
INT8U AutoCRC;
INT8U packetDelay;
INT8U i=0;


//Revision history
//v2.0 - change menu system. add single tone modulation and sequential transmit mode
//v1.5 - got the sleep mode working for the MRF24J40
//v1.6 - added option to slow down the tranmission rate of continuous transmission
//v1.7 - added option to change the number of packets used in the RSSI calculation
//v1.8 - added FIFO read tests (option 'p' under MRF menu)
//v1.9 - added test for all non-changing RAM locations
//v1.10 - set the default interpacket spacing to as fast as possible,
//          created the SEASOLVE definition to always remove the menu after reset
//v1.11 - added LO Leakage, Carrier and SideBand Suppression test
//          MRF main menu option 'q'
//v1.12 - added "Teds Tests" menu
//v1.13 - added the UBEC FIFO read workaround of clearing ShortReg0x18[7]
//v1.14 - modified to use 100us before TX or RX when warming up the PA/LNA 
//#define VERSION "v2.00"

//#define SEASOLVE

/********************** FUNCTION PROTOTYPES *************************/
void MRF24J40Init(void);
void SetChannel(INT8U channel);
void PHYSetLongRAMAddr(INT16U address, INT8U value);
void PHYSetShortRAMAddr(INT8U address, INT8U value);
INT8U PHYGetShortRAMAddr(INT8U address);
INT8U PHYGetLongRAMAddr(INT16U address);
void PHYSetDeviceAddress(INT16U PANID, INT16U shortAddress);


/********************** CONSTANTS ***********************************/
//const INT8S PredefinedPacket[] = {0x03,0x08,0xC4,0xFF,0xFF,0xFF,0xFF,0x07,0x00,0x00,0x00,0x00,0x00};
//const INT8S PERCommand[] = {0x03,0x08,0xC4,0xFF,0xFF,0xFF,0xFF,0xAA};

//1-100 only
#define RSSI_BUFFER_SIZE 50

/********************** Variables ***********************************/
volatile INT8U rssiWrite;
volatile INT8U rssiBuffer[RSSI_BUFFER_SIZE];
INT8U numRSSISamples;
INT8U input;
volatile DWORD_VAL numPacketsRx;
INT16U numPackRx = 0;
volatile DWORD_VAL PERnum;
volatile DWORD_VAL PERnumFailed;
volatile INT8U foundPERPacket;        //boolean
volatile INT8U StartSendingPER = 0;
volatile INT8U ReceivingMode;         // boolean
INT8U RxPrintMenu = 20;

typedef union
{
    INT8U txData[128];
    struct 
    {
        INT8U ShortReg[sizeof(testShortRegisters)];
        INT8U LongReg[sizeof(testLongRegisters)];
    }RegTest;
} TEST_DATA;
    
TEST_DATA testData;




// Inicialização das variáveis de acesso ao radio
void radio_init(void)
{
   extern OS_EVENT *RF_Event;
   
   for(i=0;i<RSSI_BUFFER_SIZE;i++)
   {
      rssiBuffer[i] = 0;
   }
 
   // hard reset, intialize the variables     
   AutoCRC = TRUE;     
   UseExternal = FALSE;
   ReceivingMode = OK;
    
   numPacketsRx.Val = 0;
   rssiWrite = 0;
   PERnumFailed.Val = 0;
   PERnum.Val = 0;
   foundPERPacket = FALSE;
   numRSSISamples = 1;
   
   RFIE = 1;
   PHY_WAKE = 1;
   
  OSSemCreate(0,&RF_Event);
}

// Inicializa a interrupção externa p/ o radio e os pinos de I/O
void init_radio_port(void)
{
   PTCD = 0;
   PTCDD = 0x06;
   
  /* TPM3SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=1,PS1=1,PS0=1 */
  // Bus Clock / 128
  TPM3SC = 0x0F;                       /* Stop and reset counter */
  
  // Período de 200ms
  TPM3MOD = 37500;                     /* Period value setting */
  
  (void)(TPM3C0SC);                    /* Channel 0 int. flag clearing (first part) */  
      
  /* TPM3C0SC: CH0F=0,CH0IE=1,MS0B=0,MS0A=0,ELS0B=1,ELS0A=0 */
  // Captura de entrada por borda de descida
  TPM3C0SC = 0x48;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
  //TPM3C0V = (module * DutyZero) / 100; /* Compare 0 value setting */   
}


/*********************************************************************
 * Function:        void MRF24J40Init(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    MRF24J40 is initialized
 *
 * Overview:        This function initializes the MRF24J40 and is required
 *                  before stack operation is available
 ********************************************************************/
void MRF24J40Init(void)
{  
    static INT8U i;
    static INT16U j;
    
    PHY_RESETn = 0;
    for(j=0;j<7000;j++){};

    PHY_RESETn = 1;
    for(j=0;j<7000;j++){};
 
  
    /* do a soft reset */
    PHYSetShortRAMAddr(WRITE_SOFTRST,0x07); 
    for(j=0;j<1000;j++){};
    
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
    for(j=0;j<1000;j++){};    

    
    // Enable FIFO and TX Enable
    //PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);    


    /* program the RF and Baseband Register */
    //PHYSetLongRAMAddr(RFCTRL4,0x02);
    
    /* Enable the RX */
    //PHYSetLongRAMAddr(RFRXCTRL,0x01);
    
    /* setup */    
    PHYSetLongRAMAddr(RFCTRL1,0x01);
    
    /* enable the RF-PLL */
    PHYSetLongRAMAddr(RFCTRL2,0x80);
    
    /* set TX for max output power */
    PHYSetLongRAMAddr(RFCTRL3,0x00);
    
    /* program RSSI ADC with 2.5 MHz clock */
    //PHYSetLongRAMAddr(RFCTRL6,0x04);
    PHYSetLongRAMAddr(RFCTRL6,0x90);

    //PHYSetLongRAMAddr(RFCTRL7,0b00000000);
    //PHYSetLongRAMAddr(RFCTRL7,0b10011000);
    PHYSetLongRAMAddr(RFCTRL7,0x80);
    
    PHYSetLongRAMAddr(RFCTRL8,0b00010000);
    
    
    // Disable clockout
    PHYSetLongRAMAddr(SCLKDIV,0x21);


    // Define PAN Coordinator
    //PHYSetShortRAMAddr(WRITE_RXMCR, 0b00101001);
    
    // Join network as end device
    PHYSetShortRAMAddr(WRITE_RXMCR, 0x21);
    
    // Join network as coordenator
    //PHYSetShortRAMAddr(WRITE_RXMCR, 0x25);
    
    /* flush the RX fifo */
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);    
    
    // Configure Unslotted CSMA-CA Mode
    i = PHYGetShortRAMAddr(READ_TXMCR);
    i = i & 0xDF;
    PHYSetShortRAMAddr(WRITE_TXMCR,i);
    
    // Define Nonbeacon-Enabled Network
    PHYSetShortRAMAddr(WRITE_ORDER,0xFF);



    /* Program CCA mode using RSSI */
    // 0x80 é o valor default, 0xB8 é o recomendado
    //PHYSetShortRAMAddr(WRITE_BBREG2,0xB8);
    PHYSetShortRAMAddr(WRITE_BBREG2,0xB8);
    //PHYSetShortRAMAddr(WRITE_BBREG2,0x80);
    
    /* Program CCA, RSSI threshold values */
    // Valor Default é 0x00, recomendado é 0x60
    // Determina o níve de sinal que indica o canal como ocupado
    // 0x60 = -69 dBm
    PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);
    //PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x00);            
    
    
    /* Enable the packet RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG6,0x40);
    
    /* Program the short MAC Address, 0xffff */
    PHYSetShortRAMAddr(WRITE_SADRL,0xFF);
    PHYSetShortRAMAddr(WRITE_SADRH,0xFF);
    PHYSetShortRAMAddr(WRITE_PANIDL,0xFF);
    PHYSetShortRAMAddr(WRITE_PANIDH,0xFF);
    
    /* Program Long MAC Address*/
    for(i=0;i<8;i++)
    {
        PHYSetShortRAMAddr(WRITE_EADR0+i*2,myLongAddress[i]);
    }



    //do
    //{
    //    i = PHYGetLongRAMAddr(RFSTATE);
    //}
    //while((i&0xA0) != 0xA0);
    
    
    
     
    // Disable clockout
    //i = PHYGetLongRAMAddr(SCLKDIV);      
    
    SetChannel(CHANNEL_15);
    for(j=0;j<1000;j++){};
    
    //i = PHYGetShortRAMAddr(READ_ISRSTS);
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    
    // Habilita interrupções
    PHYSetShortRAMAddr(WRITE_INTMSK,0xF6);
    
    // bypass the errata to make RF communication stable
    PHYSetLongRAMAddr(RFCTRL1, 0x01);
    
    //i = PHYGetShortRAMAddr(READ_EADR0);    
}


/*********************************************************************
 * Function:        void SetChannel(BYTE channel)
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:           BYTE channel - this is the channel that you wish
 *                  to operate on.  This should be CHANNEL_11, CHANNEL_12,
 *                  ..., CHANNEL_26. 
 *
 * Output:          None
 *
 * Side Effects:    the MRF24J40 now operates on that channel
 *
 * Overview:        This function sets the current operating channel
 *                  of the MRF24J40
 ********************************************************************/
void SetChannel(INT8U channel)
{
    
    PHYSetLongRAMAddr(RFCTRL0, (channel | 0x02));
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
}

/*********************************************************************
 * Function:        void PHYSetLongRAMAddr(WORD address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the LONG RAM address
 *                      that you want to write to
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a LONG RAM address
 ********************************************************************/
void PHYSetLongRAMAddr(INT16U address, INT8U value)
{
    int i=0;
    //volatile INT8U tmpRFIE = RFIE;
    //RFIE = 0;
    //OSEnterCritical;
    PHY_CS = 0;
    for(i=0;i<20;i++){};
    SPISendChar((((INT8U)(address>>3))&0b01111111)|0x80);
    SPISendChar((((INT8U)(address<<5))&0b11100000)|0x10);
    SPISendChar(value);
    for(i=0;i<40;i++){};
    PHY_CS = 1;
    //OSExitCritical;
    //RFIE = tmpRFIE;
}

/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40 
 *                      include file.
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a short RAM address
 ********************************************************************/
void PHYSetShortRAMAddr(INT8U address, INT8U value)
{
    int i=0;
    //volatile INT8U tmpRFIE = RFIE;
    //RFIE = 0;
    //OSEnterCritical;    
    PHY_CS = 0;
    for(i=0;i<20;i++){};       
    SPISendChar(address);
    SPISendChar(value);
    for(i=0;i<40;i++){};    
    PHY_CS = 1;
    //OSExitCritical;    
    //RFIE = tmpRFIE;
}


/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40 
 *                      include file.
 *
 * Output:          BYTE the value read from the specified short register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a short RAM address
 ********************************************************************/
INT8U PHYGetShortRAMAddr(INT8U address)
{
    int i=0;
    volatile INT8U toReturn;
    //volatile INT8U tmpRFIE = RFIE;
    //RFIE = 0;
    //OSEnterCritical;    
    PHY_CS = 0;
    for(i=0;i<20;i++){};
    SPISendChar(address);
    toReturn = SPIGet();
    //for(i=0;i<80;i++){};    
    PHY_CS = 1;
    //OSExitCritical;    
    //RFIE = tmpRFIE;
    return toReturn;
}


/*********************************************************************
 * Function:        BYTE PHYGetLongRAMAddr(WORD address)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          BYTE the value read from the specified Long register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a long RAM address
 ********************************************************************/
INT8U PHYGetLongRAMAddr(INT16U address)
{
    int i=0;
    volatile INT8U toReturn = 0;
    //volatile INT8U tmpRFIE = RFIE;
    //RFIE = 0;
    //OSEnterCritical;    
    PHY_CS = 0;
    for(i=0;i<20;i++){};
    SPISendChar(((address>>3)&0b01111111)|0x80);
    SPISendChar(((address<<5)&0b11100000));
    toReturn = SPIGet();
    for(i=0;i<20;i++){};    
    PHY_CS = 1;
    //OSExitCritical;    
    //RFIE = tmpRFIE;
    return toReturn;
}


/*********************************************************************
 * Function:        void PHYSetDeviceAddress(WORD PANID, WORD shortAddress)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD PANID is the PANID you want to operate on
 *                  WORD shortAddress is the short address you want to use on that network
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the short address for the IEEE address
 *                      filtering
 ********************************************************************/
void PHYSetDeviceAddress(INT16U PANID, INT16U shortAddress)
{
    PHYSetShortRAMAddr(WRITE_SADRL,(INT8U)(PANID&0x00FF));
    PHYSetShortRAMAddr(WRITE_SADRH,(INT8U)(PANID>>8));
    PHYSetShortRAMAddr(WRITE_PANIDL,(INT8U)(shortAddress & 0x00FF));
    PHYSetShortRAMAddr(WRITE_PANIDH,(INT8U)(shortAddress>>8));
}

void HardwareCRC(INT8U CRCmode)
{
  if (CRCmode == TRUE)
  {
    PHYSetShortRAMAddr(WRITE_RXMCR,0x23);
    AutoCRC = TRUE;  
  }else 
  {
    PHYSetShortRAMAddr(WRITE_RXMCR,0xA3);
    AutoCRC = FALSE;  
  }
}


INT8U spectral_energy(INT8U channel)
{  
    INT8U j;
    INT16U k;
    INT8U maxRSSI = 0;
    volatile INT8U RSSIcheck;
    
    // set channel
    if ((channel >= 11) && (channel <=26))
    {
      j = (INT8U)((channel-11) << 4);    // 0 = channel 11
      
      SetChannel(j);
    
      for(k=0;k<1000;k++){}; 
    
      for(j = 0; j < 200; j++)
      {  
        // set RSSI request
        PHYSetShortRAMAddr(WRITE_BBREG6, 0x80);
                        
        // check RSSI
        RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
        while((RSSIcheck & 0x01) != 0x01 || (RSSIcheck & 0x80))
        {
          RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
        }
       
        RSSIcheck = PHYGetLongRAMAddr(0x210);
                             
        if( RSSIcheck > maxRSSI )
        {
          maxRSSI = RSSIcheck;
        }
       
        for(k=0;k<1000;k++){};
                                
      }
    
      return maxRSSI;             // Return max RSSI founded
      
    }else
    {
      return CHANNEL_ERROR;       /// Not available channel
    }
    
    
}


void Read_spectral_energy(void)
{
   INT8U currentChannel = 0;
   INT8U i, j;
   INT16U k;
   INT8U maxRSSI;
   
  for(i = currentChannel; i <= 15; i++)
  {
     volatile INT8U RSSIcheck;
     volatile INT8U r[2] = {0x54, 0x00};
                           
     maxRSSI = 0;
     // set channel
     j = i << 4;
     SetChannel(j);
     
     for(k=0;k<1000;k++){};
                           
        for(j = 0; j < 200; j++)
        {
           // set RSSI request
           PHYSetShortRAMAddr(WRITE_BBREG6, 0x80);
                               
           // check RSSI
           RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
           while((RSSIcheck & 0x01) != 0x01 || (RSSIcheck & 0x80))
           {
              RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
                                    
           }
           RSSIcheck = PHYGetLongRAMAddr(0x210);
                                
           if( RSSIcheck > maxRSSI )
           {
              maxRSSI = RSSIcheck;
           }
           for(k=0;k<1000;k++){};
                               
        }
  }
}



void SendPackage(INT8U data){
  INT8U i;
  INT8U pingPong;
  INT8U txPtr;
                        
  txPtr = 0;
  pingPong = 0;  
                            
  for(i=0;i<txPtr;i++)
  {
     PHYSetLongRAMAddr(i+2, testData.txData[i]); /* Dest Address  */
  }
  
  PHYSetLongRAMAddr(0x000,txPtr);
  PHYSetLongRAMAddr(0x001,txPtr);
                                    
  PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested
}
  
  

interrupt void Radio_Interrupt(void)
{
  extern OS_EVENT *RF_Event; 
  /* Write your interrupt code here ... */

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção

  (void)TPM3C0SC;                       // Lê o registrador TPM2C0SC
  TPM3C0SC_CH0F = 0;                    // Limpa a flag CH0F do registrador TPM2C0SC
  
  OSSemPost(RF_Event);

          
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}

void Transmite_Uptime_SPI(void)
{
   INT8U caracter = 0;
   OSTime Uptime;
   OSDate UpDate;
   
   OSEnterCritical;
   Uptime = OSUptime();
   UpDate = OSUpDate();
   OSExitCritical;
   
   // Só funciona até 99 dias
   if(UpDate.RTC_Day > 0)
   {    
      if(UpDate.RTC_Day > 9)
      {
          caracter = (UpDate.RTC_Day / 10) + 48;
          OSWQueue(&SerialPortBuffer,caracter);         
      }
    
      caracter = (UpDate.RTC_Day%10) + 48;
      OSWQueue(&SerialPortBuffer,caracter);
      OSWQueue(&SerialPortBuffer,'d');
      OSWQueue(&SerialPortBuffer,' ');      
   }
   
   if(Uptime.RTC_Hour > 9)
   {   
      caracter = (Uptime.RTC_Hour / 10) + 48;
      OSWQueue(&SerialPortBuffer,caracter);
   }
   caracter = (Uptime.RTC_Hour%10) + 48;
   OSWQueue(&SerialPortBuffer,caracter);
   OSWQueue(&SerialPortBuffer,'h');
   OSWQueue(&SerialPortBuffer,' ');
   
   if(Uptime.RTC_Minute > 9)
   {   
      caracter = (Uptime.RTC_Minute / 10) + 48;
      OSWQueue(&SerialPortBuffer,caracter);
   }
   caracter = (Uptime.RTC_Minute%10) + 48;
   OSWQueue(&SerialPortBuffer,caracter);
   OSWQueue(&SerialPortBuffer,'m');
   OSWQueue(&SerialPortBuffer,' ');

   if(Uptime.RTC_Second > 9)
   {
      caracter = (Uptime.RTC_Second / 10) + 48;
      OSWQueue(&SerialPortBuffer,caracter);
   }
   caracter = (Uptime.RTC_Second%10) + 48;
   OSWQueue(&SerialPortBuffer,caracter);   
   OSWQueue(&SerialPortBuffer,'s');
   OSWQueue(&SerialPortBuffer,' ');   
   OSWQueue(&SerialPortBuffer,10);
   OSWQueue(&SerialPortBuffer,13);
}
