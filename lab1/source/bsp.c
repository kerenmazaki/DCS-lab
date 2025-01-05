//#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h" //lab kit
//#else
//#include "../header/bsp_msp430x2xx.h" //pesronal kit
//#endif

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------

/*
personal kit:
BPs- P2 bits 0-3
LEDs- P1
PWM- P2 bit 7

lab kit:
BPs- P2 bits 0-3
LEDs- P9
PWM- P2 bit 7
*/

void GPIOconfig(void) {
    // volatile unsigned int i; // in case of while loop usage

    WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

    // LEDs 8-bit Array Port configuration
    LEDsArrPortSel = 0x00;            // all bits are set to I/O
    LEDsArrPortDir = 0xFF;             // all bits are set to output 
    LEDsArrPort = 0x00;		      // clear all LEDs


    // PushButtons and PWM Setup
    PBsArrPortSel &= ~0x8F;            // bits 7,3,2,1,0 are set to I/O
    PwmOutPort |= 0x80;               // sets PWM to 1 (bit 7)
    PBsArrPortDir &= ~0x0F;           // sets bits 0-3 to input
    PBsArrPortDir |= 0x80;            // sets bit 7 to output
    PBsArrIntEdgeSel |= 0x03;  	     // bits 0,1 are set to pull-up mode
    PBsArrIntEdgeSel &= ~0x0C;         //bits 2,3 are set to pull-down mode
    PBsArrIntEn |= 0x0F;              // enable interupts
    PBsArrIntPend = 0x00;            // clear pending interrupts

    //switches
    SWsArrPortSel &= ~0x70;     // bits 4,5,6 are set to I/O
    SWsArrPortDir &= ~0x70;     // sets bits 4,5,6 to input
    SWsArrIntEn &= ~0x70;    //disable interupts


    _BIS_SR(GIE);                     // enable interrupts globally
}
//------------------------------------------------------------------------------------- 
//            Timers configuration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void) {

    //write here timers configuration code
}
//------------------------------------------------------------------------------------- 
//            ADC configuration 
//-------------------------------------------------------------------------------------
void ADCconfig(void) {

    //write here ADC configuration code
}










