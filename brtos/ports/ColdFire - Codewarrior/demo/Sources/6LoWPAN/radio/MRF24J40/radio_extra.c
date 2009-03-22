
/********************** FUNCTIONS ***********************************/
void main(void)
{   
    BYTE channel;
    BYTE power;
    BYTE mode;
    BOOL UseExternal;
    BOOL AutoCRC;
    BYTE packetDelay;
    BYTE i;
    #if defined(__18F67J11)
        char buf[32];
    #endif
    
    for(i=0;i<RSSI_BUFFER_SIZE;i++)
    {
        rssiBuffer[i] = 0;
    }

    // decide if it is a hard reset or soft reset
    if((RCON & 0b00000011) != 0b00000011)
    {   
        // hard reset, intialize the variables     
        AutoCRC = TRUE;    
        PrintMenus = FALSE;    
        UseExternal = FALSE;
        ReceivingMode = FALSE;
        RCON |= 0b00000011;
    }
    else
    {
        // soft reset, keep them the way they were
        RCON |= 0b00000011;
    }
    
    numPacketsRx.Val = 0;
    rssiWrite = 0;
    PERnumFailed.Val = 0;
    PERnum.Val = 0;
    foundPERPacket = FALSE;
    numRSSISamples = 1;
    
    // Enable PORTB pull-ups (INTCON2bits.RBPU)
    RBPU = 0;

    // Make the PORTB switch connections inputs.
    TRISB4 = 1;
    TRISB5 = 1;
    
    packetDelay = 1;
    
    #if defined(SEASOLVE)
        PrintMenus = FALSE;
    #else
    {
        PrintMenus = TRUE;
    }
    #endif
    
    BoardInit(); 
    ConsoleInit();

MainMenu:
    if(PrintMenus)
    {
        nl();
        nl();
        nl();
    }
    else
    {
        c("+");
    }

    
MRF24J40_Start: 
    
    //****************//
    if(PrintMenus)
    {
        c("Trying to initialize the MRF24J40\r\n\r\n");
    }    
    MRF24J40Init();
                        
MRF24J40_SubMenu:
    if(PrintMenus)
    {
        PrintHeader((rom char *)"Main Menu");
        c("    (a) Settings\r\n");
        c("    (b) Testings\r\n");
        c(">>");
    }
    while(!ConsoleIsGetReady())
    {
        GIEH = 1;
        if( RF_INT_PIN == 0 )
        {
            INT0IF = 1;
        } 
        input = ' ';
        if( StartSendingPER )
        {
            //goto SendingPER;     
            while(1)
            {
                BYTE i;
                
                RA1 = 1;
                //tx packets fast
                PHYSetLongRAMAddr(0x000,sizeof(PredefinedPacket));
                PHYSetLongRAMAddr(0x001,sizeof(PredefinedPacket));
                
                for(i=2;i<(sizeof(PredefinedPacket)+2);i++)
                {
                    PHYSetLongRAMAddr(i, PredefinedPacket[i-2]); /* Dest Address  */
                }

                if(UseExternal)
                {
                    PHYSetShortRAMAddr(WRITE_GPIO, 0b00000011); //TX
                    Delay100TCYx(4);
                    c("Using external PA\r\n");
                }
                //PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested 

                {
                    unsigned int blah;
                    
                    blah = 0;
                    
                    while(1)
                    {
                        DWORD j;
                        DWORD k;
                        
                        if(ConsoleIsGetReady())
                        {
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                        
                        k = ((DWORD)packetDelay)*((DWORD)packetDelay);
                        
                        do
                        {
                            for(j=0;j<2000;j++)
                            {
                            }
                        }
                        while(k--);

                        PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested 

                        blah++;
                        if((blah % 100) == 0 )
                        {
                            for(j = 0; j < 0xFFFF; j++) {}
                        }
                        if(blah == 1000)
                        {
                            break;
                        }
                    }
                }
                RA1 = 0;
                StartSendingPER = 0;
                c("Completed sending 1000 packets\r\n");
                if( UseExternal)
                {
                    PHYSetShortRAMAddr(WRITE_GPIO, 0b00001100); //RX
                    Delay100TCYx(4);
                    c("Using external LNA\r\n");   
                }
                goto MRF24J40_SubMenu;                       
            }  
        }
    }
    
MRF24J40_NewOption:        
    input = ConsoleGet();
    ConsolePut(input);
    if(PrintMenus)
    {
        nl();
    }

    switch(input)
    {
        case 'a':
        case 'A':
            if( PrintMenus )
            {
                PrintHeader((rom char *)"Settings");
                c("    (a) Set the Operating Channel\r\n");
                c("    (b) Set the Output Power\r\n");
                c("    (c) Set Hardware CRC checking\r\n");
                c("    (d) Configure External Power Amplifier and Low Noise Amplifier\r\n");
                c("    (e) Set Transmission Delay Between Packets\r\n");
                c("    (f) Set the Number of Averaged RSSI Samples\r\n");
                c("    (g) Turbo Mode Configuration\r\n");
                c(">>");
            }
            input = ConsoleInput();
            
            switch(input)
            {
                case 'a':
                case 'A':
MRF24J40_Channel_submenu:                
                    if(PrintMenus)
                    {
                        PrintHeader((rom char *)"Set the Operating Channel");
                        c("    (a) Channel 11\r\n");
                        c("    (b) Channel 12\r\n");
                        c("    (c) Channel 13\r\n");
                        c("    (d) Channel 14\r\n");
                        c("    (e) Channel 15\r\n");
                        c("    (f) Channel 16\r\n");
                        c("    (g) Channel 17\r\n");
                        c("    (h) Channel 18\r\n");
                        c("    (i) Channel 19\r\n");
                        c("    (j) Channel 20\r\n");
                        c("    (k) Channel 21\r\n");
                        c("    (l) Channel 22\r\n");
                        c("    (m) Channel 23\r\n");
                        c("    (n) Channel 24\r\n");
                        c("    (o) Channel 25\r\n");
                        c("    (p) Channel 26\r\n");
                        c("    (~) Reset Device\r\n");
                        c(">>");
                    }
                        
                    input = ConsoleInput();
                    if((input<0x61) || (input>0x70))
                    {
                        if(input == '~')
                        {
                            if(PrintMenus)
                            {
                                c("Device Reset\r\n");
                            }
                           {WORD i; for(i=0;i<30000;i++){}}
                            Reset();
                        }
                        if(PrintMenus)
                        {
                            c("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            nl();nl();
                        }
                        goto MRF24J40_Channel_submenu;
                    }
                    else
                    {
                        BYTE var;
                        
                        var = ((input-0x61)<<4);
                        PrintChar(var);
                        SetChannel(var);
                        if(PrintMenus)
                        {
                            c("Channel successfully changed\r\n");
							PrintChar(var);
                        }
                        goto MRF24J40_SubMenu;
                    }
                    break;
                    
                case 'b':
                case 'B':
MRF24J40_output_power_submenu:
                    if(PrintMenus)
                    {
                        PrintHeader((rom char *)"Set Output Power");
                        c("    (a) 0 dB\r\n");
                        c("    (b) -1.25 dB\r\n");
                        c("    (c) -2.5 dB\r\n");
                        c("    (d) -3.75 dB\r\n");
                        c("    (e) -5 dB\r\n");
                        c("    (f) -6.25 dB\r\n");
                        c("    (g) -7.5 dB\r\n");
                        c("    (h) -8.75 dB\r\n");
                        c("    ...\r\n");
                        c("    (z) -31.25 dB\r\n");
                        c("    (0) -32.5 dB\r\n");
                        c("    (1) -33.75 dB\r\n");
                        c("    (2) -35 dB\r\n");
                        c("    (3) -36.25 dB\r\n");
                        c("    (4) -37.5 dB\r\n");
                        c("    (5) -38.75 dB\r\n");
                        c(">>");
                    }
                    
                    input = ConsoleInput();
                    if((input>=0x61) && (input<=0x7A))
                    {
                        power = ((input-0x61)<<3);
                        PHYSetLongRAMAddr(RFCTRL3,power);
                        if(PrintMenus)
                        {                        
                            c("Output power successfully changed\r\n");
                        }
                        goto MRF24J40_SubMenu;
                    }
                    else if((input>=0x30) && (input<=0x35))
                    {
                        power = ((input-0x30+0x1A)<<3);
                        PHYSetLongRAMAddr(RFCTRL3,power);
                        if(PrintMenus)
                        {                        
                            c("Output power successfully changed\r\n");
                        }
                        goto MRF24J40_SubMenu;
                    }
                    else
                    {
                        if(input == '~')
                        {
                            if(PrintMenus)
                            {
                                c("Device Reset\r\n");
                            }
                            {WORD i; for(i=0;i<30000;i++){}}                            
                            Reset();
                        }                        
                        if(PrintMenus)
                        {
                            c("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            nl();nl();
                        }
                        goto MRF24J40_output_power_submenu;
                    }    
                    break;
                
                case 'c':
                case 'C':
MRF24J40_CRC_Checking_submenu:                
                    if( PrintMenus )
                    {
                        PrintHeader((rom char *)"Hardware CRC checking");
                        c("    (a) Disable CRC Checking\r\n"); 
                        c("    (b) Enable CRC Checking\r\n");
                        c(">>");
                    }
                    input = ConsoleInput();
                    switch(input)
                    {
                        case 'a':
                            PHYSetShortRAMAddr(WRITE_RXMCR,0xA3);
                            AutoCRC = FALSE;
                            if(PrintMenus)
                            {
                                c("Automatic CRC is disabled\r\n");
                            }
                            break;
                        case 'b':
                            PHYSetShortRAMAddr(WRITE_RXMCR,0x23);
                            AutoCRC = TRUE;
                            if(PrintMenus)
                            {
                                c("Automatic CRC is enabled\r\n");
                            }
                            break;
                        default:
                            c("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            goto MRF24J40_CRC_Checking_submenu;        
                    }
                    goto MRF24J40_SubMenu;
                    
                case 'd':
                case 'D':
                    PHYSetShortRAMAddr(WRITE_GPIO, 0b00001100); //RX
                    PHYSetShortRAMAddr(WRITE_GPIODIR, 0b00001111);
                    UseExternal = TRUE;
                    if(PrintMenus)
                    {
                        c("Set device to use External LNA/PA\r\nDevice reset required to remove this feature.\r\n\r\n");
                    }
                    PHYSetShortRAMAddr(WRITE_GPIO, 0b00001100); //RX
                    Delay100TCYx(4);
                    c("Using external LNA\r\n");
                    goto MRF24J40_SubMenu;

                case 'e':
                case 'E':
MRF24J40_Transmission_Delay_submenu:                
                    if(PrintMenus)
                    {
                        PrintHeader((rom char *)"Transmission Delay Between Packets");
                        c("    (0) no delay\r\n");
                        c("    (1) 1 unit of delay\r\n");
                        c("    (2) 4 units of delay\r\n");
                        c("    ...\r\n");
                        c("    (9) 81 units of delay\r\n");
                        c("    (a) 100 units of delay\r\n");
                        c("    (b) 121 units of delay\r\n");
                        c("    ...\r\n");
                        c("    (z) 1225 units of delay (approx 1 sec)\r\n");
                        c(">>");
                    }
                    input = ConsoleInput();
                    if(input<0x30)
                    {
                        if(PrintMenus)
                        {
                            c("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            nl(); nl();
                        }
                        goto MRF24J40_Transmission_Delay_submenu;
                    }
                    
                    input -= 0x30;
                    
                    if(input>9)
                    {
                        input-=0x21;
                    }
                    
                    packetDelay = input;
                    
                    goto MRF24J40_SubMenu;

                case 'f':
                case 'F':
                    if( PrintMenus )
                    {
                        PrintHeader((rom char *)"RSSI Sample Number");
                        c("    Please enter 2 decimal digits (1-49)\r\n");
                        c("      i.e. - enter 05 for 5 samples\r\n");
                        c(">>");    
                    }
_sampleRSSILoop1:
                    numRSSISamples = 0;                    
                    while(!ConsoleIsGetReady()){}                
                    input = ConsoleGet();
                    switch(input)
                    {
                        case '4':
                            numRSSISamples += 10;
                            //fall through
                        case '3':
                            numRSSISamples += 10;
                            //fall through
                        case '2':
                            numRSSISamples += 10;
                            //fall through
                        case '1':
                            numRSSISamples += 10;
                            //fall through
                        case '0':
                            break;
                        default:
                            goto _sampleRSSILoop1;
                    }
                    
                    ConsolePut(input);
                    
_sampleRSSILoop2:                    
                    while(!ConsoleIsGetReady()){}
                
                    input = ConsoleGet();
                    switch(input)
                    {
                        case '9':
                            numRSSISamples += 1;
                            //fall through
                        case '8':
                            numRSSISamples += 1;
                            //fall through
                        case '7':
                            numRSSISamples += 1;
                            //fall through
                        case '6':
                            numRSSISamples += 1;
                            //fall through                                                                                                              
                        case '5':
                            numRSSISamples += 1;
                            //fall through
                        case '4':
                            numRSSISamples += 1;
                            //fall through
                        case '3':
                            numRSSISamples += 1;
                            //fall through
                        case '2':
                            numRSSISamples += 1;
                            //fall through
                        case '1':
                            numRSSISamples += 1;
                            //fall through
                        case '0':
                            break;
                        default:
                            goto _sampleRSSILoop2;
                    }
                    
                    ConsolePut(input);
                    
                    if(numRSSISamples == 0)
                    {
                        if(PrintMenus)
                        {
                            c("\r\n\r\ndivide by 0 not allowed.  setting number of samples to 1.\r\n");
                        }
                        
                        numRSSISamples = 1;
                    }
                    
                    c("\r\n\r\n");
                    
                    goto MRF24J40_SubMenu;
                    
                case 'g':
                case 'G':
MRF24J40_Turbo_Mode_submenu:                
                    if( PrintMenus )
                    {
                        PrintHeader((rom char *)"Turbo Mode");
                        c("    (a) Enable Turbo Mode\r\n");
                        c("    (b) Disable Turbo Mode\r\n");
                        c(">>");  
                    }
                    input = ConsoleInput();
                    switch(input)
                    {
                        case 'a':
                        case 'A':
                            PHYSetShortRAMAddr(WRITE_BBREG0, 0x01);
                            PHYSetShortRAMAddr(WRITE_BBREG3, 0x38);
                            PHYSetShortRAMAddr(WRITE_BBREG4, 0x5C);
                            
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
                            if( PrintMenus )
                            {
                                c("\r\nDevice set to turbo mode.\r\n");
                            }
                            break;
                           
                       case 'b':
                       case 'B':
                            PHYSetShortRAMAddr(WRITE_BBREG0, 0x00);
                            PHYSetShortRAMAddr(WRITE_BBREG3, 0xD8);
                            PHYSetShortRAMAddr(WRITE_BBREG4, 0x9C);
                            
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
                            if( PrintMenus )
                            {
                                c("\r\nDevice set to normal mode.\r\n");
                            }
                            break;
                          
                        default:
                            if( PrintMenus )
                            {
                                c("Incorrect Menu Choice: ");
                                ConsolePut(input);
                                nl(); nl(); 
                            }
                            goto MRF24J40_Turbo_Mode_submenu;
                    }
                    break;
                    
                default:
                    goto MRF24J40_SubMenu;
            }
            break;
            
            
        case 'b':
        case 'B':
            if(PrintMenus)
            {
                PrintHeader((rom char *)"Testings");
                c("    (a) Set the Radio in Receiving Mode\r\n");
                c("    (b) Transmit Precoded Packet Continuously\r\n");
                c("    (c) Transmit Packet Defined by User\r\n");
                c("    (d) Test Low Power Mode\r\n");
                c("    (e) Energy Detection on All Channels\r\n");
                c("    (f) Test Simple Local Oscillator\r\n");
                c("    (g) Test Single Tone Modulation\r\n");
                c("    (h) Test Sequential Transmit Mode for All Channels\r\n");
                c("    (i) PER Test between Two Devices\r\n");
                c("    (z) Dump Values of Transceiver's Registers\r\n");
                c(">>");
            }
            input = ConsoleInput();
            
            switch(input)
            {
                case 'a':
                case 'A':
                    PHYSetShortRAMAddr(WRITE_RXMCR,0x23);
                    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
                    
                    if( UseExternal )
                    {
                        PHYSetShortRAMAddr(WRITE_GPIO, 0b00001100); //RX
                        Delay100TCYx(4);
                        c("Using external LNA\r\n");
                    }
                    
                    if(PrintMenus)
                    {
						nl();
                        c("Placing transceiver in receive mode\r\n");
                        c("Please reset the board to return to menu\r\n");
                        nl();
                    }
                    GIEH = 1;       //enable interrupts
                    
                    ReceivingMode = TRUE;
                
                    while(1)
                    {
                        if(ConsoleIsGetReady())
                        {
                            GIEH = 0;
                            ReceivingMode = FALSE;
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                    }
                    
                    break;
                
                case 'b':
                case 'B':
                    if(PrintMenus)
                    {
						nl();
                        ConsolePutROMString((rom char*)"Entering transmit mode.\r\n");
                        c("Press Any key to return to menu\r\n");
                        nl();
                    }
                    {
                        BYTE i;
                        
                        PHYSetLongRAMAddr(0x000,sizeof(PredefinedPacket));
                        PHYSetLongRAMAddr(0x001,sizeof(PredefinedPacket));
                        
                        for(i=2;i<(sizeof(PredefinedPacket)+2);i++)
                        {
                            PHYSetLongRAMAddr(i, PredefinedPacket[i-2]); 
                        }
                    }

                    if(UseExternal)
                    {
                        PHYSetShortRAMAddr(WRITE_GPIO, 0b00000011); //TX
                        Delay100TCYx(4);
                        c("Using external PA\r\n");
                    }
                    PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested 

        
                    while(1)
                    {
                        unsigned int j;
                        DWORD k;
                        
                        if(ConsoleIsGetReady())
                        {
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                        
                        k = ((DWORD)packetDelay)*((DWORD)packetDelay);
                        
                        do
                        {
                            for(j=0;j<200;j++)
                            {
                            }
                        }
                        while(k--);

                        PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested 
                    }

                    break;
                
                
                case 'c':
                case 'C':
                    if(PrintMenus)
                    {
                        c("Please enter the packet you want to transmit.\r\n");
                        c("Use all uppercase HEX values starting from the frame control (no length)\r\n");
                        c(" Press '=' when you are done.\r\n");
                        c(">>");
                    }
                    {
                        BYTE pingPong;
                        BYTE txPtr;
                        BYTE A,B;
                        
                        txPtr = 0;
                        pingPong = 0;
                        
                        while(1)
                        {
MRF24J40_read_tx:
                            while(!ConsoleIsGetReady()){}    
                            input = ConsoleGet();
                            
                            switch(input)
                            {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    ConsolePut(input);
                                    B = (input - '0');
                                    break;
                                case 'A':
                                case 'B':
                                case 'C':
                                case 'D':
                                case 'E':
                                case 'F':
                                    ConsolePut(input);
                                    B = (input - 'A' + 0x0A);
                                    break;
                                case '~':
                                    if(PrintMenus)
                                    {
                                        c("Device Reset");
                                    }
                                    {WORD i; for(i=0;i<30000;i++){}}
                                    Reset();
                                    break;
                                case '=':
                                    if(PrintMenus)
                                    {
                                        c("\r\n\r\nTransmitting Packet.\r\nReturning to the main menu.\r\n\r\n");
                                    }
                                    {
                                        BYTE i;
                                        
                                        for(i=0;i<txPtr;i++)
                                        {
                                            PHYSetLongRAMAddr(i+2, testData.txData[i]); /* Dest Address  */
                                        }
                                    }
                                    PHYSetLongRAMAddr(0x000,txPtr);
                                    PHYSetLongRAMAddr(0x001,txPtr);
                                    
                                    if(UseExternal)
                                    {
                                        PHYSetShortRAMAddr(WRITE_GPIO, 0b00000011); //TX
                                        Delay100TCYx(4);
                                        c("Using external PA\r\n");
                                    }
                                    
                                    PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);  //transmit packet without ACK requested 
                                                                        
                                    goto MRF24J40_SubMenu;
                                    break;
                                default:
                                    goto MRF24J40_read_tx;
                                    break;
                            }
                            if(pingPong == 1)
                            {
                                testData.txData[txPtr++] = A+B;
                                pingPong = 0;
                            }
                            else
                            {
                                A = B<<4;
                                pingPong = 1;
                            }
                        }
                    }
                    goto MRF24J40_SubMenu;
                
                case 'd':
                case 'D':
MRF24J40_Low_Power_submenu:                
                    if( PrintMenus )
                    {
                        PrintHeader((rom char *)"Test Low Power Mode");
                        c("    (a) Disable Pullups\r\n");
                        c("    (b) Place MRF in sleep mode\r\n");
                        c("    (c) Place MRF in reset\r\n");
                        c("    (d) Place Controller to sleep\r\n");
                        c("    (e) Wake up the MRF from sleep mode \r\n");
                        c(">>");
                    }
                    input = ConsoleInput();
                    switch(input)
                    {
                        case 'a':
                        case 'A':
                            if(PrintMenus)
                            {
                                nl();
								c("Pullups disabled\r\n");
								c("Reset the device to get out of disabled pullups\r\n");
								nl();
                                RBPU = 1;
                            }
                            break;
                        
                        case 'b':
                        case 'B':
                            if(PrintMenus)
                            {
                                c("MRF24J40 in sleep mode\r\n");
								c("Reset the device to get out of sleep mode\r\n");
                            }
                            
                            RC1 = 0;
                            TRISC1 = 0;
                            
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
                            
                            PHYSetShortRAMAddr(WRITE_TXBCNINTL,0x80);
                            PHYSetShortRAMAddr(WRITE_RXFLUSH,0b01100000);
                            PHYSetShortRAMAddr(WRITE_SLPACK,0x80);
                            break;
                        
                        case 'c':
                        case 'C':
                            if(PrintMenus)
                            {
                                c("MRF24J40 in reset mode\r\n");
                            }
                            
                            PHY_RESETn = 0;
                                
                            break;
                        
                        case 'd':
                        case 'D':
                            if(PrintMenus)
                            {
								nl();
                                c("Placing controller in sleep\r\n");
                                c("Reset the device to get out of sleep mode\r\n");
                            }
                            {
                                unsigned int i;
                                
                                for(i=0;i<60000;i++)
                                {
                                    Nop();
                                }
                                Sleep();
                            }
                            break;
                        
                        case 'e':
                        case 'E':
                            if(PrintMenus)
                            {
                                c("Waking up the MRF24J40\r\n");
                            }
                            RC1 = 1;
                            break;
                        
                        default:
                            if( PrintMenus )
                            {
                                c("Incorrect Menu Choice: ");
                                ConsolePut(input);
                                nl(); nl();
                            }
                            goto MRF24J40_Low_Power_submenu;
                        
                    }
                    break;
                
                case 'e':
                case 'E':
                     {
                        BYTE currentChannel = 0;
                        BYTE i, j;
                        WORD k;
                        BYTE maxRSSI;

                        c("Please wait!\r\n");
						c("Spectral energy values will be displayed for each channel in RSSI units:\r\n");

						nl(); nl();
                        for(i = currentChannel; i <= 15; i++)
                        {
                            BYTE RSSIcheck;
                            BYTE r[2] = {0x54, 0x00};
                            
                            maxRSSI = 0;
                            // set channel
                            j = i << 4;
                            SetChannel(j);
                            for(k = 0; k < 0xFFF; k++); 
                            
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
                                for(k = 0; k < 0xFFF; k++);
                                
                            }
                            ConsolePutROMString((ROM char *)"Channel ");

							if(i>=9)
							{
								ConsolePut('2');
							}
							else
							{
								ConsolePut('1');
							}

							{
								unsigned a;

								a = 0x30 + ((i+1)%10);
								ConsolePut(a);
							}

                            //PrintChar(i+11);
                            ConsolePutROMString((ROM char *)": ");
                            j = maxRSSI/5;
                            for(k = 0; k < j; k++)
                            {
                                ConsolePut('-');
                            }
                            c(" ");
                            PrintChar(maxRSSI);
                            ConsolePutROMString((ROM char *)"\r\n");
                        }
                        c("Press any key to return to the main menu\r\n");
                        while(!ConsoleIsGetReady()){}
                        input = ConsoleGet();
                        goto MRF24J40_SubMenu;
                    
                    } 
                    break;                
                    
                case 'f':
                case 'F':
                    if(PrintMenus)
                    {
                        c("Running in CW mode\r\n");
                        c("Reset board to return to the main menu\r\n");
                        nl();
                    }
                           
                    PHYSetLongRAMAddr(0x000, 22); /* packet header length */
                    PHYSetLongRAMAddr(0x001, 23); /* total packet length (not including the FCS/CRC or length) */
                    PHYSetLongRAMAddr(0x002, 0b01100001); /* first byte of packet (the LSB of the frame control p112 of IEEE 802.15.4-2003) -- Data packet, no security, no frame pending, ACK requested, intra-pan transmission */ 
                    PHYSetLongRAMAddr(0x003, 0xCC); /* first byte of packet (the MSB of the frame control p112 of IEEE 802.15.4-2003), use long addresses for both cases */     
                    PHYSetLongRAMAddr(0x004, 0x01); /* IEEE sequence number */
                    PHYSetLongRAMAddr(0x005, 0xFF); /* PANID - broadcast */
                    PHYSetLongRAMAddr(0x006, 0xFF); /* PANID - broadcast */
                    PHYSetLongRAMAddr(0x007, 0x01); /* Dest Address LSB */
                    PHYSetLongRAMAddr(0x008, 0x23); /* Dest Address  */
                    PHYSetLongRAMAddr(0x009, 0x45); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00a, 0x67); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00b, 0x89); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00c, 0xab); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00d, 0xcd); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00e, 0xef); /* Dest Address MSB */
                    PHYSetLongRAMAddr(0x00f, EUI_0); /* Source Address LSB */
                    PHYSetLongRAMAddr(0x010, EUI_1); /* Source Address  */
                    PHYSetLongRAMAddr(0x011, EUI_2); /* Source Address  */
                    PHYSetLongRAMAddr(0x012, EUI_3); /* Source Address  */
                    PHYSetLongRAMAddr(0x013, EUI_4); /* Source Address  */
                    PHYSetLongRAMAddr(0x014, EUI_5); /* Source Address  */
                    PHYSetLongRAMAddr(0x015, EUI_6); /* Source Address  */
                    PHYSetLongRAMAddr(0x016, EUI_7); /* Source Address MSB */
                    PHYSetLongRAMAddr(0x017, 0x55); /* data byte */
                    PHYSetLongRAMAddr(0x018, 0xAA); /* data byte */
             
                    PHYSetShortRAMAddr(WRITE_RFCTL,0b00000010);  //force transmit
                    
                    while(1)
                    {
                        if(ConsoleIsGetReady())
                        {
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                    }
                    break;                
                
                
                case 'g':
                case 'G':
                    {
                        BYTE currentChannel = CHANNEL_11;
                        c("\r\nEntering Single Tone Test Mode. Reset device to return to the main menu.");
                        
                        while(1)
                        {
                            SetChannel(currentChannel);
                            currentChannel += 0x10;
                            
                            PHYSetLongRAMAddr(RFCTRL3,0x00);
                            PHYSetShortRAMAddr(WRITE_RFCTL,0b00000010);
                            PHYSetLongRAMAddr(TESTMODE,0x0d); 
                            Delay10KTCYx(255);
                            Delay10KTCYx(255);
                        }
                    }
                    break;
                
                case 'h':
                case 'H':
                    {
                        BYTE currentChannel = CHANNEL_11;
                        c("\r\nEntering Sequential Transmit Mode. Reset device to return to the main menu.");
                        
                        while(1)
                        {
                            BYTE i;
                            WORD j;
                            
                            SetChannel(currentChannel);
                            currentChannel += 0x10;
                            
                            PHYSetLongRAMAddr(RFCTRL3,0x00);
                            PHYSetShortRAMAddr(WRITE_RXMCR, 0b00100001);    //promiscuous mode (no address filtering) [bit 0], never send ACKs [bit 5]
                            GIEH = 1;
                            PHYSetLongRAMAddr(0x000, 12); /* packet header length */
                            PHYSetLongRAMAddr(0x001, 16); /* total packet length (not including the FCS/CRC or length) */
                            PHYSetLongRAMAddr(0x002, 0x00); /* data byte */
                            PHYSetLongRAMAddr(0x003, 0x01); /* data byte */
                            PHYSetLongRAMAddr(0x004, 0x02); /* data byte */
                            PHYSetLongRAMAddr(0x005, 0x03); /* data byte */
                            PHYSetLongRAMAddr(0x006, 0x04); /* data byte */
                            PHYSetLongRAMAddr(0x007, 0x05); /* data byte */
                            PHYSetLongRAMAddr(0x008, 0x06); /* data byte */
                            PHYSetLongRAMAddr(0x009, 0x07); /* data byte */
                            PHYSetLongRAMAddr(0x00a, 0x08); /* data byte */
                            PHYSetLongRAMAddr(0x00b, 0x09); /* data byte */
                            PHYSetLongRAMAddr(0x00c, 0x0a); /* data byte */
                            PHYSetLongRAMAddr(0x00d, 0x0b); /* data byte */
                            PHYSetLongRAMAddr(0x00e, 0x0c); /* data byte */
                            PHYSetLongRAMAddr(0x00f, 0x0d); /* data byte */
                            PHYSetLongRAMAddr(0x010, 0x0e); /* data byte */
                            PHYSetLongRAMAddr(0x011, 0x0f); /* data byte */
                            
                            for (i=0;i<400;i++)
	                        {
		                        PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);
		                        for(j = 0; j < 5000; j++) ;
		                    }
                        }
                    }
                    break;

                
                case 'i':
                case 'I':
                    {
                        BYTE i;
                        
                        PHYSetLongRAMAddr(0x000,sizeof(PERCommand));
                        PHYSetLongRAMAddr(0x001,sizeof(PERCommand));
                                    
                        for(i=2;i<(sizeof(PERCommand)+2);i++)
                        {
                            PHYSetLongRAMAddr(i, PERCommand[i-2]); 
                        }
                        
                        if(UseExternal)
                        {
                            PHYSetShortRAMAddr(WRITE_GPIO, 0b00000011); //TX
                            Delay100TCYx(4);
                            c("Using external PA\r\n");
                        }
                        GIEH = 1;       //enable interrupts
                        PHYSetShortRAMAddr(WRITE_TXNMTRIG,0b00000001);
                        
                        numPacketsRx.Val = 0;
                        if(UseExternal)
                         {
                             PHYSetShortRAMAddr(WRITE_GPIO, 0b00001100); 
                             Delay100TCYx(4);
                             c("Using external LNA\r\n");
                         }
                         
                         numPacketsRx.Val = 0;
                         while(1)
                         {
                             WORD k;
                             if( RB0 == 0 )
                             {
                                 INT0IF = 1;
                             }
                             for(k = 0; k < 50; k++) {}
                             #if defined(__18F67J11)
                                sprintf(buf, (far const rom char*)"%d packets received\r", numPacketsRx.word.LW);
                                for(i = 0; i < 32; i++)
                                {
                                    if( buf[i] == 0x00 )
                                    {
                                        break;
                                    }
                                    ConsolePut(buf[i]);
                                }
                             #else
                                printf((far const rom char*)"%d packets received\r",numPacketsRx.word.LW);
                             #endif
                             if(ConsoleIsGetReady())
                             {
                                 ConsoleGet();
                                 break;
                             }
                         }
                         GIEH = 0;       //enable interrupts
                         //print results
                         #if defined(__18F67J11)
                            sprintf(buf, (far const rom char*)"%d packets received\r\n", numPacketsRx.word.LW);
                            for(i = 0; i < 32; i++)
                            {
                                if( buf[i] == 0x00 )
                                {
                                    break;
                                }
                                ConsolePut(buf[i]);
                            }
                         #else
                            printf((far const rom char*)"%d packets received\r\n",numPacketsRx.word.LW);
                         #endif
                    }
                    break;
                
                
                
                case 'z':
                case 'Z':
                    {
                        BYTE i,data;
                        
                        c("-- Short RAM Addresses\r\nAddress\tValue\r\n");
                        for(i=0x00;i<=0x3F;i++)
                        {
                            data = PHYGetShortRAMAddr(i<<1);
                            c("Register 0x");
                            PrintChar(i);
                            c(":\t");
                            PrintChar(data);
                            c("\r\n");
                            if( (i & 0x0F) == 0x0F )
                            {
                                c("Press Any Key to Continue>>");
                                ConsoleInput();
                            }
                        }
                        
                        c("\r\n-- Long RAM Addresses\r\nAddress\tValue\r\n");
                        for(i=0x00;i<=0x4C;i++)
                        {
                            data = PHYGetLongRAMAddr(0x200+i);
                            c("Register 0x2");
                            PrintChar(i);
                            c(":\t");
                            PrintChar(data);
                            c("\r\n");
                            if( (i & 0x0F) == 0x0F )
                            {
                                c("Press Any Key to Continue>>");
                                ConsoleInput();
                            }
                           
                        }
                        c("\r\n");                        
                    }
                    goto MRF24J40_SubMenu;
                    break;
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
    goto MRF24J40_SubMenu;
}


/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized
 *
 * Overview:        This function configures the board for the PICDEM-z
 *                  MRF24J40 usage 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize MiWi stack or any other function that
 *                  operates on the stack
 ********************************************************************/
void BoardInit(void)
{
    #if defined(__18F4620)
    // Switches S2 and S3 are on RB5 and RB4 respectively. We want interrupt-on-change
    INTCON = 0x00;

    // There is no external pull-up resistors on S2 and S3. We will use internal pull-ups.
    // The MRF24J40 is using INT0 for interrupts
    // Enable PORTB internal pullups
    INTCON2 = 0x00;
    INTCON3 = 0x00;

    // Make PORTB as input - this is the RESET default
    TRISB = 0xff;

    // Set PORTC control signal direction and initial states
    // disable chip select
    LATC = 0xfd;

    // Set the SPI module for use by Stack
    TRISC = 0xD0;

    // Set the SPI module
    SSPSTAT = 0xC0;
    SSPCON1 = 0x20;

    // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
    // Make PORTA as digital I/O.
    // The TC77 temp sensor CS is on RA2.
    ADCON1 = 0x0F;

    // Deselect TC77 (RA2)
    LATA = 0x04;

    // Make RA0, RA1, RA2 and RA4 as outputs.
    TRISA = 0xF8;
    
    PHY_CS = 1;             //deselect the MRF24J40
    PHY_CS_TRIS = 0;        //make chip select an output   
    
    RFIF = 0;               //clear the interrupt flag
    
    if(RB0 == 0)
    {
        RFIF = 1;
    }
    
    RFIE = 1;               //set interrupt enable flag

    IPEN = 1;
    
    GIEH = 1;
    #else
    //WDTCONbits.SWDTEN = 0; //disable WDT

    OSCCON = 0b01110110;
    OSCTUNE = 0b01000000;
    
    // Switches are on RB1 to RB4 respectively. We don't want interrupt-on-change
    INTCON = 0x00;
    
    // There is no external pull-up resistors on S2 and S3. We will use internal pull-ups.
    // The MRF24J40 is using INT0 for interrupts
    // Enable PORTB internal pullups
    INTCON2 = 0x80;
    INTCON3 = 0x00;

    // Make PORTB as input - this is the RESET default
    TRISB = 0xff;

    // Make PORTE as output - this drives LEDs
    LATE = 0x00;
    TRISE = 0x00;

    // Set PORTC control signal direction and initial states
    // disable chip select
    //LATC = 0xfd;
    LATD = 0b11111011;

    // Set the SPI module for use by Stack
    //TRISC = 0xD0;
    TRISD = 0b00100000;

    // Set the SPI module
    SSP2STAT = 0xC0;
    SSP2CON1 = 0x20;

    // Deselect TC77 (RA2)
    //LATA = 0x04;
    //PORTA = 0x00;
    //LATA = 0x00;
    
    TRISA = 0xFF;
    
    // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
    // Make PORTA as digital I/O.
    // The TC77 temp sensor CS is on RA2.
    //ADCON1 = 0x0F;
    //ADCON0 = 0xCC;
    WDTCONbits.ADSHR = 1;
    ANCON0 = 0xDC;
    ANCON1 = 0xFF;
    WDTCONbits.ADSHR = 0;
    ADCON0 = 0x05;
    ADCON1 = 0xBA;
    //ADCON1 = 0xB9;
    //WDTCONbits.ADSHR = 0;
    
    // Make RA0, RA1, RA2 and RA4 as outputs.
    //TRISA = 0xF8;
    //TRISA = 0xFF;
    
    //DelayMs(1);
    //ADCON1bits.ADCAL = 1;
    //ADCON0bits.GO = 1;
    //while(ADCON0bits.GO);
    //ADCON1bits.ADCAL = 0;
    
    PHY_CS = 1;             //deselect the MRF24J40
    PHY_CS_TRIS = 0;        //make chip select an output   
    
    RFIF = 0;               //clear the interrupt flag

    RFIE = 1;
    
    //RCONbits.IPEN = 1;
    IPEN = 1;
    
    INTCON2bits.INTEDG0 = 0;

    GIEH = 1;
    
    // enable LCD
    
    LATF = 0x00;
    TRISF = 0x00;
    #endif
}


/*********************************************************************
 * Function:        void HighISR(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Various flags and registers set.
 *
 * Overview:        This is the interrupt handler for the MRF24J40 and
 *                  MiWi stack.
 ********************************************************************/
#if defined(MCHP_C18)
    #pragma interruptlow HighISR
    void HighISR(void)
#elif defined(HITECH_C18)
    void interrupt HighISR(void)
#else
    void HighISR(void)
#endif
{       
    if(RFIE && RFIF)
    {                    
        //clear the interrupt flag as soon as possible such that another interrupt can
        //occur quickly.
        RFIF = 0;
        
        //create a new scope for the MRF24J40 interrupts so that we can clear the interrupt
        //flag quickly and then handle the interrupt that we have already received
        {
            MRF24J40_IFREG flags;        

            //read the interrupt status register to see what caused the interrupt        
            flags.Val=PHYGetShortRAMAddr(READ_ISRSTS);
            //PrintChar(flags.Val);
           
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
                BYTE i,j,k,RSSI_VAL;
                WORD p;
                DWORD_VAL v;
                   
                //packet received
                //Need to read it out
                
                i=PHYGetLongRAMAddr(0x300); //first byte of the receive buffer is the packet length (this 
                j=PHYGetLongRAMAddr(0x308);
                numPacketsRx.Val++;

                RSSI_VAL = PHYGetLongRAMAddr(0x300+i+2);
                
                if(j == 0xAA )
                {
                    StartSendingPER = 1; 
                }
                else
                {
                    if( ReceivingMode )
                    {
						if(RxPrintMenu++ == 20)
						{
							c("\r\nRSSI | Packet Length | Packet Data\r\n\r\n");
							RxPrintMenu = 0;
						}
                        PrintChar(RSSI_VAL);
						c(" | ");
                        PrintChar(i);   //print the packet length
						c(" | ");
                        for(j=1;j<=i;j++)
                        {
                            PrintChar(PHYGetLongRAMAddr(0x300 + j));    //read out the rest of the buffer
                        }
                        c("\r\n");
                    }
                }
                
                rssiBuffer[rssiWrite++] = RSSI_VAL;
                if(rssiWrite >= numRSSISamples)
                {
                    rssiWrite = 0;
                }
                PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
            } //end of RXIF check
        } //end of scope of RF interrupt handler
    } //end of if(RFIE && RFIF)
    
} //end of interrupt handler

//******************************************************************************
// Interrupt Vectors
//******************************************************************************
#if defined(MCHP_C18)
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

#if defined(MCHP_C18)
#pragma code lowhVector=0x18
void LowVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif