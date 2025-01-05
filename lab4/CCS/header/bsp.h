#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430FG4619.h>  // MSP430x4xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms

// RGB abstraction
//#define RGBArrPortOut      P2OUT

//// LEDs abstraction
//#define LEDsArrPort        P9OUT
//#define LEDsArrPortDir     P9DIR
//#define LEDsArrPortSel     P9SEL

// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL


// RGB
#define RGBPortSel        P2SEL
#define RGBPortDir        P2DIR
#define RGBPortOut        P2OUT
#define B                 0x01
#define G                 0x02
#define R                 0x04


////   Generator abstraction
//#define GenPort            P2IN
//#define GenPortSel         P2SEL
//#define GenPortDir         P2DIR
//#define GenPortOut         P2OUT

//   Buzzer abstraction
#define BuzzPortSel        P2SEL
#define BuzzPortDir        P2DIR
#define BuzzPortOut        P2OUT

// potentiometer abstraction
#define PotPortSel        P1SEL
#define PotPortDir        P1DIR
#define PotPortIn         P1IN

// RGB abstraction
#define RGBArrPortSel        P2SEL
#define RGBArrPortDir        P2DIR
#define RGBArrIntEn          P2IE
#define RGBOut               P2OUT

//USCI abstraction
#define  UARTSel            P1SEL
#define  UARTSel2           P1SEL2
#define  UARTDir            P1DIR
#define  UARTOut            P1OUT

//ADC abstraction


//// Switches abstraction
//#define SWsArrPort         P2IN
//#define SWsArrPortDir      P2DIR
//#define SWsArrPortSel      P2SEL
//#define SWsArrIntEn	       P2IE
//
// PushButtons abstraction
#define PBsArrPort	       P1IN
#define PBsArrPortOut      P1OUT
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	       P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB1                0x01

////  Keypad abstraction
//#define KeypadPortSel         P10SEL
//#define KeypadPortDIR         P10DIR
//#define KeypadPortOUT         P10OUT
//#define KeypadPortIN          P10IN
//
//#define KeypadIRQPort         P2IN
//#define KeypadIRQIntPend      P2IFG
//#define KeypadIRQIntEn        P2IE
//#define KeypadIRQIntEdgeSel   P2IES
//#define KeypadIRQPortSel      P2SEL
//#define KeypadIRQPortDir      P2DIR
//#define KeypadIRQPortOut      P2OUT



#endif
