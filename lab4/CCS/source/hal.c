#include  "../header/bsp.h"   		// private library - API layer
#include  "../header/hal.h"   		// private library - API layer
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define ASCII_offset 48
const char string1[] = {"I love my Negev\r\n"};
const char string2[] = {"Howdy Mate\r\n"};
const char state4str[] = {"Enter delay time:"};
unsigned int i, k = 0;
int x_load = 0;
char X_char[16];
int send_7 = 0;

//---------------------------------------------------------------------
//            KEY PAD
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//            DMA
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
// 
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE){
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else{
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
    int place = 0;
    while(*s){
        lcd_data(*s++);
        place ++;
        if (place == 16) {
            lcd_new_line();
        }
        if (place == 32) {
        break;
        }
    }
}

//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);
    
    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  __no_operation();
  __no_operation();
//  asm("NOP");
//  asm("NOP");
  LCD_EN(0);
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) __no_operation();;//asm("nop"); // the command asm("nop") takes 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){
  
	unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}

//---------------------------------------------------------------------
//            hald sec delay
//---------------------------------------------------------------------
void Half_Sec(){
  TACCR0=0xFFFF;
  TACTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
  __bis_SR_register(LPM0_bits + GIE);
}

//---------------------------------------------------------------------
//            X msecs delay
//---------------------------------------------------------------------
void X_delay(){
    int t;
    int times = floor(X / 500);
    int reminder = (X % 500);
    for (t = 0; t < times; t++) { //if X is larger than 500 ms
        Half_Sec();
    }
    if (reminder != 0){
        TACCR0=0xFFFF * (float)reminder * 0.002; // = X/500
        TACTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
        __bis_SR_register(LPM0_bits + GIE);
    }
}


//---------------------------------------------------------------------
//            Enter LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}


//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}


//--------------------------------------------------------------------
//             System Configuration
//--------------------------------------------------------------------
void sysConfig(void){
    GPIOconfig();
    USCIconfig();
    TIMERconfig();
    ADCconfig();
    lcd_init();
    lcd_clear();
}

//---------------------------------------------------------------------
//            USCI interrupts
//---------------------------------------------------------------------

//---------------------------------------------------------------------
//            TX interrupt
//---------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (state == state4){
        UCA0TXBUF = state4str[i++];
        if (i == sizeof state4str - 1)              // TX over?
          IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
      }
    else if (state == state7){
      UCA0TXBUF = string1[i++];                 // TX next character

        if (i == sizeof string1 - 1){              // TX over?
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
            send_7 = 0;
        }
    }

    else {
      UCA0TXBUF = string2[i++];                 // TX next character

        if (i == sizeof string2 - 1){              // TX over?
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
        }
    }
}

//---------------------------------------------------------------------
//            RX interrupt
//---------------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  if (UCA0RXBUF == 'u')                     // 'u' received?
  {
    i = 0;
    IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
    UCA0TXBUF = string2[i++];
  }

  else if(UCA0RXBUF == '#'){
      memset(X_char, 0, sizeof(X_char));
      x_load = 1;
      k = 0;
 }
  else if (x_load == 1){

      if (UCA0RXBUF == '*'){
          x_load = 0;
          k = 0;
          //X = 0;
//          unsigned int l;
//          for (l = 0; X_char[l] != '\0'; l++) {
//              X = X * 10 + (X_char[l] - ASCII_offset);
//          }
          X = atoi(X_char);
      }
      else{
          X_char[k] = UCA0RXBUF;
          k++;
      }
  }

  else{
    switch (UCA0RXBUF) {
        case '1':
            state = state1;
            break;

        case '2':
            state = state2;
            break;

        case '3':
            state = state3;
            break;

        case '4':
            state = state4;
            i = 0;
            IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
            UCA0TXBUF = state4str[i++];
            break;

        case '5':
            state = state5;
            break;

        case '6':
            state = state6;
            break;

        case '7':
            state = state7;
            break;

        case '8':
            state = state8;
            break;

        case '9':
            state = state9;
            break;
    }

// Exit from LPM0 mode

        switch(lpm_mode){
        case mode0:
            LPM0_EXIT; // must be called from ISR only
            break;

        case mode1:
            LPM1_EXIT; // must be called from ISR only
            break;

        case mode2:
            LPM2_EXIT; // must be called from ISR only
            break;

        case mode3:
            LPM3_EXIT; // must be called from ISR only
            break;

        case mode4:
            LPM4_EXIT; // must be called from ISR only
            break;
        }
  }
}
//*********************************************************************
//            TimerA0 Interrupt Service Routine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{

    LPM0_EXIT;
    TACTL = MC_0+TACLR; //turn timer off after half a second
}

//*********************************************************************
//            TimerA1 Interrupt Service Routine, for state1
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA1IV, 0x0A))
  {
      case  TA1IV_NONE: break;
      case  TA1IV_TACCR1: break;
      case TA1IV_TACCR2: break;
      default:  break;
  }
}

//*********************************************************************
//            ADC10 Vector Interrupt Service Routine, for state3
//*********************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}

//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
    delay(debounceVal);

//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------

      if((PBsArrIntPend & PB1) && state == state7){
          send_7 = 1;
          i = 0;
          IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
          UCA0TXBUF = string1[i++];
          PBsArrIntPend &= ~PB1;
        }
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
        switch(lpm_mode){
        case mode0:
         LPM0_EXIT; // must be called from ISR only
         break;

        case mode1:
         LPM1_EXIT; // must be called from ISR only
         break;

        case mode2:
         LPM2_EXIT; // must be called from ISR only
         break;

        case mode3:
         LPM3_EXIT; // must be called from ISR only
         break;

        case mode4:
         LPM4_EXIT; // must be called from ISR only
         break;
    }
  }
