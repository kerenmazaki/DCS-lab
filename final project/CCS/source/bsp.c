#include  "../header/bsp.h"
#include  "../header/LCD.h"

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

  //motor
  MotPortSel &= ~0x0F;            // I/O mode
  MotPortDir |= 0x0F;             // P2.0-3 Output

  //joystick
  JoyPortSel &= ~0x09;            // I/O mode
  JoyPortDir &= ~0x09;            // P1.0 Vrx and P1.3 Vry Input

  JoyPressSel &= ~JoyPress;                 // I/O mode
  JoyPressDir &= ~JoyPress;                 // Input
  JoyPressIntEdgeSel |= JoyPress;           // pull-down mode
  JoyPressREN |= JoyPress;                  // pullup/pulldown enabled
  JoyPressIntEn &= ~JoyPress;               //  disable interrupts, enable only when entering relevant states
  JoyPressIntPend &= ~JoyPress;            //  clear pending interrupts
  JoyPressOUT |= JoyPress;                 // Set output to high (pull-up resistor)


  //UART
  UARTSel = 0x06;                     // P1.1 = RXD, P1.2=TXD
  UARTSel2 = 0x06;                     // P1.1 = RXD, P1.2=TXD

  _BIS_SR(GIE);                     // enable interrupts globally
}   

//------------------------------------------------------------------------------------- 
//            Timers configuration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
    //for X ms count
    TA0CCTL0 = CCIE;
}

//-------------------------------------------------------------------------------------
//            USCI configuration
//-------------------------------------------------------------------------------------
void USCIconfig(void){
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
    UCA0BR0 = 104;
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS0;                       // 0x02
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + SREF_0 + ADC10IE;  // 16*ADCLK, ADC10ON, Vcc & Vss as reference
    ADC10AE0 |= BIT0 + BIT3;  // Enable A0 (Vrx) and A3 (Vry) as analog inputs
}

int str_to_int(char digit){
    switch (digit) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': return 10;
        case 'B': return 11;
        case 'C': return 12;
        case 'D': return 13;
        case 'E': return 14;
        case 'F': return 15;
        default: return -1;
    }
}

