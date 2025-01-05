#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  
  // LCD
  LCD_DATA_WRITE &= ~0xFF; // clear LCD port
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P1.4-P1.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7

  // RGB
  RGBArrPortSel &= ~0x07;            // I/O mode
  RGBArrPortDir |= 0x07;            // set to output
  RGBArrIntEn &= ~0x07;               // P2.0-3 disable interrupts
  RGBOut &= ~0x07;                //clear output

  // Buzzer
  BuzzPortDir |= 0x10;             // P2.2 Output compare - '1'
  BuzzPortSel |= 0x10;             // P2.2 Select = '1'
  BuzzPortOut &= ~0x10;             // P2.2 out = '0'

  //POT
  PotPortSel &= ~0x08;            // I/O mode
  PotPortDir |= 0x08;             // P1.3 - Output ('1')

  //UART
  UARTSel = 0x06;                     // P1.1 = RXD, P1.2=TXD
  UARTSel2 = 0x06;                     // P1.1 = RXD, P1.2=TXD

  //PB1
  PBsArrPortSel &= ~0x01;            //I/O mode
  PBsArrPortDir &= ~0x01;            // P1.0-3 - Input ('0')
  PBsArrIntEdgeSel |= 0x01;          // pull-up mode PB1
  PBsArrIntEn |= 0x01;               // P1.0-3 enable interrupts
  PBsArrIntPend &= ~0x01;            // clear pending interrupts P1.0-P1.3 all P1


  _BIS_SR(GIE);                     // enable interrupts globally
}   

//------------------------------------------------------------------------------------- 
//            Timers configuration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
    
    //for X ms count
    TA0CCTL0 = CCIE;

    //for state 3 PWM
    TA1CCTL2 =  OUTMOD_7; //+ CCIE; // TA1CCR1 reset/set;
}


//-------------------------------------------------------------------------------------
//            USCI configuration
//-------------------------------------------------------------------------------------
void USCIconfig(void){
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
    UCA0BR0 = 104;                           //
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS0;               // 0x02
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
      ADC10CTL0 = ADC10SHT_2 + ADC10ON + SREF_0 + ADC10IE;  // 16*ADCLK+ Turn on, set ref to Vcc and Gnd, and Enable Interrupt
      ADC10CTL1 = INCH_3 + ADC10SSEL_3;         // Input A3 and SMCLK, was |
      ADC10AE0 |= BIT3;                         // P1.3 ADC option select
}

