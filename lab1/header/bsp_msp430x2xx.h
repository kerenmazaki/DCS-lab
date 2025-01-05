#ifndef _BSP_H_2xx_
#define _BSP_H_2xx_ 

/* pesronal kit

BPs- P2 bits 0-3
LEDs- P1
PWM- P2 bit 7
*/

#include  <msp430g2553.h>          // MSP430x2xx

#define debounceVal      250
//one period is 0.954 us
#define LEDs_RATE  511000   // 0.5 sec
#define TPwm 20

// LEDs abstraction
#define LEDsArrPort        P1OUT   //Evalution Kit
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F

// PushButtons abstraction
#define PBsArrPort         P2IN
#define PBsArrIntPend      P2IFG
#define PBsArrIntEn        P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL
#define PBsArrPortDir      P2DIR
#define PB0                0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

// PWM abstraction
#define PwmOutPort         P2OUT

// duty cycle
#define delay_0  2 //25%
#define delay_1 16 //75%
// 18 cycles for 4kHz
#endif




