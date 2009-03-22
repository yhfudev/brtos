#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "drivers.h"
#include "BRTOS.h"
#include "event.h"
#include "queue.h"

INT8U receive_byte = 0;

INT8U reset1 = 0;
INT8U reset2 = 0;

// Declara um ponteiro para o bloco de controle da porta serial
OS_EVENT *SerialResource;


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
 
void PWM3Setup(INT16U Frequency, INT16U Align, INT8U ChannelZero, INT16U DutyZero,INT16U PolarityZero, 
                                         INT8U ChannelOne,   INT16U DutyOne,   INT16U PolarityOne,
                                         INT8U ChannelTwo,   INT16U DutyTwo,   INT16U PolarityTwo,
                                         INT8U ChannelThree, INT16U DutyThree, INT16U PolarityThree,
                                         INT8U ChannelFour,  INT16U DutyFour,  INT16U PolarityFour,
                                         INT8U ChannelFive,  INT16U DutyFive,  INT16U PolarityFive)
{
  INT16U module = 0;  
  INT8U prescale = 0;
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
  TPM3SC = 0x00;                       /* Stop and reset counter */
  TPM3MOD = module;                    /* Period value setting */
  
  if (ChannelZero == 1)
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
  
  if (ChannelOne == 1)
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
  
  if (ChannelTwo == 1)
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
     
  (void)(TPM3SC == 0);                 /* Overflow int. flag clearing (first part) */
  /* TPM3SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=0,PS1=0,PS0=0 */
  
  if (Align == LeftAlign) 
    TPM3SC = 0x08 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */
    
  if (Align == CenterAlign) 
    TPM3SC = 0x28 + prescale;            /* Int. flag clearing (2nd part) and timer control register setting */
    
}


interrupt void serial_tx(void)
{
  // ************************
  // Entrada de interrupção
  // ************************
  
  //asm(MOVE.w #0x2700,SR);

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção
  
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
  
}

//Função para adquirir direito exclusivo a porta serial
// Assim que possível colocar recurso de timeout
void acquireSCI2(void)
{
  // Aloca o recurso da porta serial
  OSMutexPend (SerialResource, 0);
}

//Função para liberar a porta serial
void releaseSCI2(void)
{
  // Libera o recurso da porta serial
  OSMutexPost(SerialResource);
}


void Serial_Envia_Caracter(byte caracter)
{
  
  while (!SCI2S1_TC){};
  SCI2D = caracter;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
  
}

void Serial_Envia_Frase(char *string)
{

  while(*string)
  { 
    while (!SCI2S1_TC){};
    SCI2D = *string;                    /* Armazena o caracter a ser transmitido no registrador de transmissão */
    string++;
  }

}


interrupt void serial_rx(void)
{
extern OS_QUEUE SerialPortBuffer;
extern OS_EVENT *Serial;

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */
  
  
  receive_byte = SCI2D;                /* Leitura dos dados recebidos */
  
  if (OSWQueue(&SerialPortBuffer,receive_byte) == BUFFER_UNDERRUN)
  {
    // Problema: Estouro de buffer
    OSCleanQueue(&SerialPortBuffer);
  }
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
  // ************************
  
}

interrupt void serial_erro(void)
{
  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção
  (void)SCI2S1;                        /* Leitura do registrador SCI1S1 para analisar o estado da transmissão */
  (void)SCI2C3;                        /* Leitura do registrador SCI1C3 para limpar o bit de paridade */
  
  // A próxima ação depende do tipo de erro
  // Devemos analisar o erro ocorrido e tomar uma das atitudes abaixo
  (void)SCI2D;                         // Leitura do registrador SCI1D para limpar os indicativos de erro.
  SCI2D = 0;                           // Escrita no registrador SCI1D para limpar os indicativos de erro. 
  
  // ************************
  // Saída de interrupção
  // ************************
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}

void init_SCI2(INT8U priority)
{  
  // Configuração da Porta Serial
  /* SCI1C1: LOOPS=0,SCISWAI=0,RSRC=0,M=1,WAKE=0,ILT=0,PE=1,PT=0 */
  // 9 Bits com paridade par
  // Baud Rate = 19200
  SCI2C1 = 0x12;               /* Configure the SCI */ 
  /* SCI1C3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  SCI2C3 = 0x00;               /* Disable error interrupts */ 
  /* SCI1S2: ??=0,??=0,??=0,??=0,??=0,BRK13=0,??=0,RAF=0 */
  SCI2S2 = 0x00;                
  /* SCI1C2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=0,RE=0,RWU=0,SBK=0 */
  SCI2C2 = 0x00;               /* Disable all interrupts */ 
  
  // 0x4E = 19200 bauds
  // 0x20 = 38400 bauds
  // 0x16 = 57600 bauds
  SCI2BDH = 0x00;                      /* Set high divisor register (enable device) */
  SCI2BDL = 0x4E;                      /* Set low divisor register (enable device) */
      /* SCI1C3: ORIE=1,NEIE=1,FEIE=1,PEIE=1 */
  SCI2C3 |= 0x0F;                      /* Enable error interrupts */
  //SCI1C2 |= ( SCI1C2_TE_MASK | SCI1C2_RE_MASK | SCI1C2_RIE_MASK); /*  Enable transmitter, Enable receiver, Enable receiver interrupt */
  SCI2C2 = 0x2C;

  // Cria um mutex com contador = 1, informando que o recurso está disponível
  // após a inicialização
  // Prioridade máxima a acessar o recurso = priority
  if (OSMutexCreate(&SerialResource,priority) != ALLOC_EVENT_OK)
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


interrupt void Keyboard(void)
{ 
  /* Write your interrupt code here ... */
  extern OS_EVENT *Teclado;

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
  
  OS_INT_EXIT_CF();
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}


interrupt void IRQ(void)
{ 
  /* Write your interrupt code here ... */
  //extern OS_EVENT *Teclado;
  extern INT16U iIRQ;

  // ************************
  // Entrada de interrupção
  // ************************

  OS_SAVE_CONTEXT();

  iNesting++;
  
  //SaveCurrentSP();
  
  // ************************  


  // Tratamento da interrupção
  IRQSC_IRQACK = 1;
  //iIRQ++;
  
  // ************************
  // Saída de interrupção
  // ************************
  
  //OS_INT_EXIT_CF();
  iNesting--;
  
  OS_RESTORE_CONTEXT();
  
// ************************  
}

void PortSetup(void)
{
  PTHD = 1;
  PTHDD = 0x03;
  PTHDS = 0x03;
}

void SerialReset(void)
{
  acquireSCI2();
  reset1 = SRS/100;
  Serial_Envia_Caracter(reset1+48);
  
  reset2 = ((SRS - reset1*100)/10);
  Serial_Envia_Caracter(reset2+48);
  
  reset1 = ((SRS - reset1*100 - reset2*10)%10);
  Serial_Envia_Caracter(reset1+48);
  Serial_Envia_Caracter(10);
  Serial_Envia_Caracter(13);
  releaseSCI2();
}