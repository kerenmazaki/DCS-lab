#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF; // clear LCD port
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  
  // Generator Setup
  //From the table at CCIx p2.4
  GenPortDir &=  ~BIT4;              // P2.4 Input Capture = '1'
  GenPortSel |=  BIT4;              // P2.4 Select = '1'

  // Buzzer Setup
  BuzzPortDir |= BIT2;             // P2.2 Output compare - '1'
  BuzzPortSel |= BIT2;             // P2.2 Select = '1'
  BuzzPortOut &= ~BIT2;             // P2.2 out = '0'

  // PushButtons Setup
  PBsArrPortSel &= ~0x07;            //I/O mode
  PBsArrPortOut &= ~0x07;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x07;            // P1.0-2 - Input ('0')
  PBsArrPortDir |= 0x08;             // P1.3 - Output ('1')
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x07;               // P1.0-2 - '1'
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1 
  
  //PB3 Setup
  PB3ArrPortSel &= ~0x02;            //I/O mode
  PB3ArrPortDir &= ~0x02;            // P2.1 - Input ('0')
  PB3ArrIntEdgeSel &= ~0x02;         // pull-down mode  P2.1 - '0'
  PB3ArrIntEn |= 0x02;               // P2.1 - '1'
  PB3ArrIntPend &= ~0x02;            // clear pending interrupts P2.1  
  
  // Switches Setup
  SWsArrPortSel &= ~0x01;            // I/O mode
  SWsArrPortDir &= ~0x01;            // P2.0 - Input ('0')
  SWsArrIntEn &= ~0x01;               // disable Sws interrupts   
    
  _BIS_SR(GIE);                     // enable interrupts globally
}   

//------------------------------------------------------------------------------------- 
//            Timers configuration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
  
    //Timer1_A1_config, state1
    TA1CCTL1 =  OUTMOD_7; // TA1CCR1 reset/set;
    
    // Timer1_A0_config, state2
    TA0CCTL0 = CCIE;
    TACCR0 = 0xFFFF;
    TA0CTL = TASSEL_2 + MC_0 + ID_3;  //  SMCLK, Up/Down  ; divider: /8
    
    
    //Timer1_A2_config, state3
      TA1CCTL2 = CAP + CM_1 + CCIE + SCS + CCIS_0; // Capture,  rising edge, enable cc interrupts, synchronized, CCIxA input
} 

//------------------------------------------------------------------------------------- 
//            ADC configuration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
      ADC10CTL = ADC10SHT_2 + ADC10ON+ SREF_0 + ADC10IE;  // 16*ADCLK+ Turn on, set ref to Vcc and Gnd, and Enable Interrupt
      ADC10CTL1 = INCH_3 + ADC10SSEL_3;     // Input A3 and SMCLK, was |
      ADC10AE0 |= BIT3;                         // P1.3 ADC option select
}
