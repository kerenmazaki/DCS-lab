#include  "../header/hal.h" 

//#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h" //lab kit
//#else
//#include "../header/bsp_msp430x2xx.h" //pesronal kit
//#endif

extern int press_count;

void sysConfig(void) {
  GPIOconfig();
  TIMERconfig();
  ADCconfig();
}

void print2LEDs(unsigned char ch) {
  LEDsArrPort = ch;
}

void clrLEDs(void) {
  LEDsArrPort = 0x000;
}

void clearPWM() {  // clear P2.7
  PwmOutPort &= 0x7F;
}

void toggele() {
  PwmOutPort ^= 0x80;
}

unsigned char readSWs(void) {
  unsigned char ch;
  ch = PBsArrPort;
  ch &= SWmask;     // mask the least 4-bit
  return ch;
}

void incLEDs(char val) {
  LEDsArrPort += val;
}

void delay(unsigned int t) {
  volatile unsigned int i;
  for (i = t; i > 0; i--);
}

void createPWM(int Nfreq, int uptime){
    if (uptime == 0) {
        clearPWM();
    }
    else if (uptime == Nfreq) {
        PwmOutPort |= 0x80;
    }
    else {
     delay(Nfreq - uptime);
     toggele();
     delay(uptime);
     toggele();
    }
}

void enterLPM(unsigned char LPM_level) {
  if (LPM_level == 0x00)
    _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
  else if (LPM_level == 0x01)
    _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
  else if (LPM_level == 0x02)
    _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
  else if (LPM_level == 0x03)
    _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
  else if (LPM_level == 0x04)
    _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}

void enable_interrupts() {
	_BIS_SR(GIE);
}

void disable_interrupts() {
	_BIC_SR(GIE);
}

#pragma vector=PORT2_VECTOR

__interrupt void PBs_handler(void) {
    delay(debounceVal);

    // selector of transition between states
    if (PBsArrIntPend & PB0) {
        state = state1;
        PBsArrIntPend &= ~PB0;
    }
    else if (PBsArrIntPend & PB1) {
        state = state2;
        PBsArrIntPend &= ~PB1;
    }
    else if (PBsArrIntPend & PB2) {
        state = state3;
        PBsArrIntPend &= ~PB2;
    }
    else if (PBsArrIntPend & PB3) {
        state = state4;
        PBsArrIntPend &= ~PB3;
    }
    else {
      state = state0;
    }

    // Exit from a given LPM 
    switch (lpm_mode) {
        case mode0: 
            LPM0_EXIT; // must be called from ISR only
            break;
        case mode1: {
            LPM1_EXIT; // must be called from ISR only
            break;
        }
        case mode2: {
            LPM2_EXIT; // must be called from ISR only
            break;
        }
        case mode3: {
            LPM3_EXIT; // must be called from ISR only
            break;
        }
    }
}


