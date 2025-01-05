#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430FG4619.h>  // MSP430x4xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms

//USCI abstraction
#define  UARTSel            P1SEL
#define  UARTSel2           P1SEL2
#define  UARTDir            P1DIR
#define  UARTOut            P1OUT

// motor abstraction
#define MotPortDir          P2DIR
#define MotPortSel          P2SEL
#define MotPortOut          P2OUT
#define phaseA              (0x08 | JoyPress)
#define phaseB              (0x04 | JoyPress)
#define phaseC              (0x02 | JoyPress)
#define phaseD              (0x01 | JoyPress)

// joystick abstraction
#define JoyPortDir          P1DIR
#define JoyPortSel          P1SEL
#define JoyPortIN           P1IN

#define JoyPressDir         P2DIR
#define JoyPressSel         P2SEL
#define JoyPressREN         P2REN
#define JoyPressOUT         P2OUT
#define JoyPressIntPend     P2IFG
#define JoyPressIntEn       P2IE
#define JoyPressIntEdgeSel  P2IES
#define JoyPress            0x10  //P1.4

// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL

#define LCD_CTL_SEL        P2SEL




enum FSMstate {state0, state1, state2, state3, state4}; // FSM states
enum SYSmode {mode0, mode1, mode2, mode3, mode4}; // Sleep modes

int str_to_int(char digit);

extern enum FSMstate state;
extern enum SYSmode lpm_mode;

typedef struct {
    int file_count;
    char* file_names[3];    // Array of 3 pointers to char
    char* file_pointers[3]; // Array of 3 pointers to char (pointers to file data)
    int file_sizes[3];      // Array of 3 integers
} FlashFileSystem;

extern FlashFileSystem script_handler;

void USCIconfig(void);
void TIMERconfig(void);
void GPIOconfig(void);
void ADCconfig(void);

#endif
