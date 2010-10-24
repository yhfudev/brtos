/**
* \file drivers.c
* \brief Microcontroller drivers
*
* This file contain the main microcontroller drivers: A/D, PWM, serial and keyboard.
*
**/

#include "hardware.h"
#include "drivers.h"
#include "BRTOS.h"
#include "event.h"
#include "queue.h"

#pragma warn_implicitconv off

INT8U receive_byte = 0;

INT8U reset1 = 0;
INT8U reset2 = 0;

// Declara um ponteiro para o bloco de controle da porta serial
BRTOS_Mutex *SerialResource;
BRTOS_Sem   *SerialTX;

extern BRTOS_Queue *Serial;


////////////////////////////////////////////////
////////////////////////////////////////////////
///    Rotinas do A/D                        ///
////////////////////////////////////////////////
////////////////////////////////////////////////

 
void ADSetup(INT8U SpeedConverter, INT8U SampleTime, INT8U NumBits)
{
  INT8U ADconfig = 0;
  
  // Configuração do A/D
  /* ADC1SC2: ADACT=0,ADTRG=0,ACFE=0,ACFGT=0,??=0,??=0,??=0,??=0 */
  ADCSC2 = 0x00;              /* Disable HW trigger and autocompare */
  
  if(SpeedConverter == HighSpeed)
    ADconfig = ADconfig & 0x7F;
  
  if(SpeedConverter == LowPower)
    ADconfig = ADconfig | 0x80;
  
  
  if ((configCPU_CLOCK_HZ) < 8000000)
  {  
    ADconfig = ADconfig & 0x9F;
  }
  else
  {
    if ((configCPU_CLOCK_HZ >> 1) < 8000000)
    {  
      ADconfig = ADconfig | 0x20;
    } else
    {
      if ((configCPU_CLOCK_HZ >> 2) < 8000000)
      {
        ADconfig = ADconfig | 0x40;
      }else
      {
        if ((configCPU_CLOCK_HZ >> 3) < 8000000)
          ADconfig = ADconfig | 0x60;
      }
    }
  }
    
  
  if(SampleTime == ShortSampleTime)
    ADconfig = ADconfig & 0xEF;
  
  if(SampleTime == LowPower)
    ADconfig = LongSampleTime | 0x10;
  
  switch(NumBits)
  {
    case 8:
      ADconfig = ADconfig & 0xF3;
      break;
    case 10:
      ADconfig = ADconfig | 0x08;
      break;
    case 12:
      ADconfig = ADconfig | 0x04;
      break;
    default:
      break;    
  }
        
  
  /* ADC1CFG: ADLPC=0,ADIV1=1,ADIV0=0,ADLSMP=0,MODE1=1,MODE0=0,ADICLK1=0,ADICLK0=0 */
  ADCCFG = ADconfig;              /* Set prescaler bits */ 
  /* ADC1SC1: COCO=0,AIEN=0,ADCO=0,ADCH4=1,ADCH3=1,ADCH2=1,ADCH1=1,ADCH0=1 */
  ADCSC1 = 0x1F;              /* Disable the module */
}




INT16U converte(INT8U canal)
{
  word return_ad = 0;
  ADCSC1 = (INT8U)(canal);    /* Inicia a medida de um canal */
  while(!ADCSC1_COCO)
  {
  };
  
  return_ad = ADCR;
  ADCSC1 = 0x1F;
  return return_ad;
}





////////////////////////////////////////////////
////////////////////////////////////////////////
///    Rotinas do Módulo PWM TPM3            ///
////////////////////////////////////////////////
////////////////////////////////////////////////
 
void PWMSetup(INT8U TPM_Number, INT16U Frequency, INT16U Align,
              INT8U ChannelZero, INT16U DutyZero,INT16U PolarityZero, 
              INT8U ChannelOne,   INT16U DutyOne,   INT16U PolarityOne,
              INT8U ChannelTwo,   INT16U DutyTwo,   INT16U PolarityTwo,
              INT8U ChannelThree, INT16U DutyThree, INT16U PolarityThree,
              INT8U ChannelFour,  INT16U DutyFour,  INT16U PolarityFour,
              INT8U ChannelFive,  INT16U DutyFive,  INT16U PolarityFive)
{
  INT16U module = 0;  
  INT8U prescale = 0;
  INT32U duty = 0;
  INT8U mode = 0;
/* ### Init_TPM init code */
  
  for (prescale = 0;prescale <=7;prescale++)
  {
    if (Align == LeftAlign)
      module = (configCPU_CLOCK_HZ / Frequency) >> prescale;
    
    if (Align == CenterAlign)
      module = (configCPU_CLOCK_HZ / Frequency) >> (prescale + 1);
  
    if (module < 0xFFFF)
       break;
  }
  
  
  /* TPM3SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  switch(TPM_Number)
  {
    case 1:
      TPM1SC = 0x00;                       /* Stop and reset counter */
      TPM1MOD = module;                    /* Period value setting */
      break;      
    case 2:
      TPM2SC = 0x00;                       /* Stop and reset counter */
      TPM2MOD = module;                    /* Period value setting */
      break;
    case 3:
      TPM3SC = 0x00;                       /* Stop and reset counter */
      TPM3MOD = module;                    /* Period value setting */
      break;
    default:
      TPM2SC = 0x00;                       /* Stop and reset counter */
      TPM2MOD = module;                    /* Period value setting */
      break;                  
  }
  
  if (ChannelZero == 1)
  {
    if (TPM_Number == 1) 
    {
      (void)(TPM1C0SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityZero == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM1C0SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM1C0V = (module * DutyZero) / 100; /* Compare 0 value setting */
    }
    
    if (TPM_Number == 2) 
    {
      (void)(TPM2C0SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityZero == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM2C0SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM2C0V = (module * DutyZero)/100;
    }
    
    if (TPM_Number == 3) 
    {
      (void)(TPM3C0SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityZero == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM3C0SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM3C0V = (module * DutyZero) / 100; /* Compare 0 value setting */
    }        
  }
  
  if (ChannelOne == 1)
  {
    if (TPM_Number == 1) 
    {  
      (void)(TPM1C1SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityOne == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;    

      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM1C1SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM1C1V = (module * DutyOne) / 100; /* Compare 0 value setting */
    }
    
    if (TPM_Number == 2) 
    {  
      (void)(TPM2C1SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityOne == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;    

      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM2C1SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM2C1V = (module * DutyOne) / 100; /* Compare 0 value setting */
    }
    
    if (TPM_Number == 3) 
    {  
      (void)(TPM3C1SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityOne == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;    

      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM3C1SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM3C1V = (module * DutyOne) / 100; /* Compare 0 value setting */
    }        
  }
  
  if (ChannelTwo == 1)
  {
    if (TPM_Number == 1) 
    {  
      (void)(TPM1C2SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityTwo == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM1C2SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM1C2V = (module * DutyTwo) / 100; /* Compare 0 value setting */
    }
    
    if (TPM_Number == 2) 
    {  
      (void)(TPM2C2SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityTwo == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM2C2SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM2C2V = (module * DutyTwo) / 100; /* Compare 0 value setting */
    }
          
    if (TPM_Number == 3) 
    {  
      (void)(TPM3C2SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
      if(PolarityTwo == PositiveDutyCyclePolarity)
        mode = 0x28;
      else
        mode = 0x24;
      
      /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
      TPM3C2SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
      TPM3C2V = (module * DutyTwo) / 100; /* Compare 0 value setting */
    }
  }
  
  if (ChannelThree == 1)
  {
    (void)(TPM3C3SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
    if(PolarityThree == PositiveDutyCyclePolarity)
      mode = 0x28;
    else
      mode = 0x24;
      
    /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
    TPM3C3SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
    TPM3C3V = (module * DutyThree) / 100; /* Compare 0 value setting */
  }
  
  if (ChannelFour == 1)
  {
    (void)(TPM3C4SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
    if(PolarityFour == PositiveDutyCyclePolarity)
      mode = 0x28;
    else
      mode = 0x24;
      
    /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
    TPM3C4SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
    TPM3C4V = (module * DutyFour) / 100; /* Compare 0 value setting */
  }
  
  if (ChannelFive == 1)
  {
    (void)(TPM3C5SC == 0);               /* Channel 0 int. flag clearing (first part) */  
    
    if(PolarityFive == PositiveDutyCyclePolarity)
      mode = 0x28;
    else
      mode = 0x24;
      
    /* TPM3C0SC: CH0F=0,CH0IE=0,MS0B=1,MS0A=0,ELS0B=0,ELS0A=1 */
    TPM3C5SC = mode;                     /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
    TPM3C5V = (module * DutyFive) / 100; /* Compare 0 value setting */
  }          
     
  switch(TPM_Number)
  {
    case 1:
      (void)TPM1SC;                       /* Stop and reset counter */
      if (Align == LeftAlign) 
        TPM1SC = 0x08 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */
      if (Align == CenterAlign) 
        TPM1SC = 0x28 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */      
      break;
    case 2:
      (void)TPM2SC;                       /* Stop and reset counter */
      if (Align == LeftAlign) 
        TPM2SC = 0x08 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */
      if (Align == CenterAlign) 
        TPM2SC = 0x28 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */      
      break;
    case 3:
      (void)TPM3SC;                       /* Stop and reset counter */
      if (Align == LeftAlign) 
        TPM3SC = 0x08 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */
      if (Align == CenterAlign) 
        TPM3SC = 0x28 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */      
      break;
    default:
      (void)TPM2SC;                       /* Stop and reset counter */
      break;                  
  }
  /* TPM3SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
 
    
}


#if (NESTING_INT == 1)
#pragma TRAP_PROC
void serial_tx(void)
#else
interrupt void serial_tx(void)
#endif
{
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();

  // Tratamento da interrupção  
  SCI2C2_TCIE = 0;
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */  
  
  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif  
  
  (void)OSSemPost(SerialTX);
  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT();  
  // ************************
}

//Função para adquirir direito exclusivo a porta serial
// Assim que possível colocar recurso de timeout
void acquireSCI2(void)
{
  // Aloca o recurso da porta serial
  OSMutexAcquire(SerialResource);
}

//Função para liberar a porta serial
void releaseSCI2(void)
{
  // Libera o recurso da porta serial
  OSMutexRelease(SerialResource);
}


void Serial_Envia_Caracter(byte caracter)
{
  
  //while (!SCI2S1_TC){};
  SCI2D = caracter;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
  SCI2C2_TCIE = 1;
  (void)OSSemPend(SerialTX,5);
  
}

void Serial_Envia_Frase(CHAR8 *string)
{

  while(*string)
  { 
    //while (!SCI2S1_TC){};
    SCI2D = *string;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
    SCI2C2_TCIE = 1;
    (void)OSSemPend(SerialTX,5);
    string++;
  }

}


#if (NESTING_INT == 1)
#pragma TRAP_PROC
void serial_rx(void)
#else
interrupt void serial_rx(void)
#endif
{
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();

  // Tratamento da interrupção
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */    
  receive_byte = SCI2D;                /* Leitura dos dados recebidos */
  
  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif  
  
  if (OSQueuePost(Serial, receive_byte) == BUFFER_UNDERRUN)
  {
    // Problema: Estouro de buffer
    OSCleanQueue(Serial);
  }
  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT();  
  // ************************
}


#if (NESTING_INT == 1)
#pragma TRAP_PROC
void serial_erro(void)
#else
interrupt void serial_erro(void)
#endif
{
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();

  // Tratamento da interrupção
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */
  
  // A próxima ação depende do tipo de erro
  // Devemos analisar o erro ocorrido e tomar uma das atitudes abaixo
  (void)SCI2D;                         // Leitura do registrador SCI1D para limpar os indicativos de erro.
  SCI2D = 0;                           // Escrita no registrador SCI1D para limpar os indicativos de erro. 
  
  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif  
  
  // ************************
  // Interrupt Exit
  // ************************
  OS_INT_EXIT();  
  // ************************
}

void init_SCI2(INT8U priority)
{  
  SCI2C1 = 0x12;               /* Configure the SCI */ 
  /* SCI1C3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */  
  SCI2C3 = 0x00;               /* Disable error interrupts */ 
  /* SCI1S2: ??=0,??=0,??=0,??=0,??=0,BRK13=0,??=0,RAF=0 */
  SCI2S2 = 0x00;                
  /* SCI1C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=0,RE=0,RWU=0,SBK=0 */
  SCI2C2 = 0x00;               /* Disable all interrupts */ 
  
  // 0x4E = 19200 bauds at 24Mhz /////  0x52 = 19200 bauds at 25.33Mhz
  // 0x20 = 38400 bauds at 24Mhz /////  0x29 = 38400 bauds at 25.33Mhz
  // 0x16 = 57600 bauds at 24Mhz /////  0x1B = 57600 bauds at 25.33Mhz
  SCI2BDH = 0x00;                      /* Set high divisor register (enable device) */
  SCI2BDL = 0x1B;                      /* Set low divisor register (enable device) */        /* Configure the SCI */ 
      /* SCI1C3: ORIE=1,NEIE=1,FEIE=1,PEIE=1 */
  SCI2C3 |= 0x0F;                      /* Enable error interrupts */
  //SCI1C2 |= ( SCI1C2_TE_MASK | SCI1C2_RE_MASK | SCI1C2_RIE_MASK); /*  Enable transmitter, Enable receiver, Enable receiver interrupt */
  SCI2C2 = 0x2C;
  
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2S2;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */  

  // Cria um mutex com contador = 1, informando que o recurso está disponível
  // após a inicialização
  // Prioridade máxima a acessar o recurso = priority
  if (OSMutexCreate(&SerialResource,priority) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };  
  
  if (OSSemCreate(0, &SerialTX) != ALLOC_EVENT_OK)
  {
    // Oh Oh
    // Não deveria entrar aqui !!!
    while(1){};
  };  
}


void Keyb2Setup(INT8U keys)
{  
  /* ### Init_KBI init code */
  /* KBI2SC: KBIE=0 */
  KBI2SC &= (INT8U)~0x02;                     
  /* KBI2ES: KBEDG7=0,KBEDG6=0,KBEDG5=0,KBEDG4=0,KBEDG3=0,KBEDG2=0,KBEDG1=0,KBEDG0=0 */
  KBI2ES = 0x00;                                      
  /* KBI2SC: KBIMOD=0 */
  KBI2SC &= (INT8U)~0x01;                     
  
  // Activate Pull-up and Interrupt keys
  PTDPE = keys;
  KBI2PE = keys;
  
  /* KBI2SC: KBACK=1 */
  KBI2SC |= (INT8U)0x04;                               
  /* KBI2SC: KBIE=1 */
  KBI2SC |= (INT8U)0x02; 
}


/*
interrupt void Keyboard(void)
{ 
  // Write your interrupt code here
  extern BRTOS_Sem *Teclado;

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção
  KBI2SC_KBIE = 0;
  KBI2SC_KBACK = 1;
    
  OSSemPost(Teclado);
  
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}
*/


void PortSetup(void)
{
  // Inicializa portas p/ display e LEDs
  PTED = 0;
  PTEDD = PTEDD | 0b11110000;
  PTHDS = PTEDS | 0b11110000;
  PTHD = 0;
  PTHDD = 0x03;
  PTHDS = 0x03;
  PTGD = 0;
  PTGDD = 0x03;
  PTFD = 0;
  PTFDD = 0xFF;
}

void SerialReset(void)
{ 
      reset1 = SRS/100;
      Serial_Envia_Caracter(reset1+48);
  
      reset2 = ((SRS - reset1*100)/10);
      Serial_Envia_Caracter(reset2+48);
  
      reset1 = ((SRS - reset1*100 - reset2*10)%10);
      Serial_Envia_Caracter(reset1+48);
      Serial_Envia_Caracter(10);
      Serial_Envia_Caracter(13);
      __RESET_WATCHDOG();
}