#ifndef _BSP_H_4xx_
#define _BSP_H_4xx_

/*lab kit

BPs- P2 bits 0-3
LEDs- P9
PWM- P2 bit 7
*/

#include  <msp430FG4619.h>  // MSP430x4xx    (MSP430FG4619)

#define debounceVal      250
//one period is 0.125 ms
#define LEDs_RATE  511000  // 0.5 sec
#define TPwm 20

// LEDs abstraction
#define LEDsArrPort        P9OUT   
#define LEDsArrPortDir     P9DIR
#define LEDsArrPortSel     P9SEL

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F
#define SWsArrIntPend       P2IFG
#define SWsArrIntEn         P2IE
#define SWsArrIntEdgeSel    P2IES

// PushButtons abstraction
#define PBsArrPort	        P2IN
#define PBsArrIntPend       P2IFG
#define PBsArrIntEn	        P2IE
#define PBsArrIntEdgeSel    P2IES
#define PBsArrPortSel       P2SEL
#define PBsArrPortDir       P2DIR
#define PB0                 0x01
#define PB1                0x02
#define PB2                0x04
#define PB3                0x08

// PWM abstraction
#define PwmOutPort         P2OUT

// duty cycle
#define delay_0  3 //25%
#define delay_1 18 //75%
// 21 cycles for 4kHz

#endif




