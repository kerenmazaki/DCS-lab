#include  "../header/bsp.h"    // private library - BSP layer

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF; // clear LCD port
  LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  
  // Keypad Setup
  KeypadPortSel &= ~0xFF;
  KeypadPortDIR = 0x0F; //10.0-10.3 output, 10.4-10.7 input
  KeypadPortOUT = 0x00; // CLR output

  KeypadIRQPortSel &= ~BIT1;
  KeypadIRQPortDir &= ~BIT1;             // P2.1 input
  KeypadIRQIntEdgeSel |= BIT1;         // pull-up mode  P2.1 - '1'
  KeypadIRQIntEn |= BIT1;               // P2.1 - '1'
  KeypadIRQIntPend &= ~0xFF;            // clear pending interrupts P2.1

  // LEDs configuration
  LEDsArrPortSel = 0x00;            // all bits are set to I/O
  LEDsArrPortDir = 0xFF;             // all bits are set to output
  LEDsArrPort = 0x00;               // clear all LEDs

  // PushButtons Setup
  PBsArrPortSel &= ~0x0F;            //I/O mode
  PBsArrPortOut &= ~0x0F;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x0F;            // P1.0-3 - Input ('0')
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x0F;               // P1.0-3 enable interrupts
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1 

    
  _BIS_SR(GIE);                     // enable interrupts globally
}   

//------------------------------------------------------------------------------------- 
//            Timers configuration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
    
    //for state 3
    TACCTL2 = ~CAP + CM_0 + ~CCIE; // compare mode, no capture, disable interrupts so it's a trigger to DMA
} 


