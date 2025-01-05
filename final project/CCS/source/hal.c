#include  "../header/bsp.h"
#include  "../header/hal.h"
#include  "../header/LCD.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"



#define ADC_SAMPLES 4
#define ADC_MAX_VALUE 1023
#define VCC 3.3f
#define X_MIN 0.0032
#define X_CENTER 1.5742
#define X_MAX 3.0355
#define Y_MIN 0.0032
#define Y_CENTER 1.5581
#define Y_MAX 3.0355
#define PI 3.14159

const char string1[] = { "Hello World\r\n" };
unsigned int tx_index = 0;
unsigned int rx_index = 0;
int u = 0;
int state3_start = 0;
char in_string[100];
char out_string[50];
int done_sending = 0;
char state2_mode = 'w';
int state1_start = 0;
int d = 10;
int script_number = 0;
char state4_mode = 'n'; //n- nothing, r- run script, w- write script

float angle;
float motor_angle = 0;

int steps = 510;
float step_angle = 0.0123;


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

//--------------------------------------------------------------------
//             motor functions
//--------------------------------------------------------------------
void step_counter_clockwise(void){

    MotPortOut = phaseA; //8 msecs = 125 Hz
    msecs_delay(2);
    MotPortOut = phaseD;
    msecs_delay(2);
    MotPortOut = phaseC;
    msecs_delay(2);
    MotPortOut = phaseB;
    msecs_delay(2);
}

void step_clockwise(void){

    MotPortOut = phaseD; //8 msecs = 125 Hz
    msecs_delay(5);
    MotPortOut = phaseA;
    msecs_delay(5);
    MotPortOut = phaseB;
    msecs_delay(5);
    MotPortOut = phaseC;
    msecs_delay(5);
}

//--------------------------------------------------------------------
//             joystick functions
//--------------------------------------------------------------------
void get_angle(void) {
    float posX, posY, Vrx, Vry;
    unsigned int adcX, adcY;
    unsigned long sum = 0;
    unsigned int i;

    // Read X-axis (Vrx on P1.0)
    for (i = ADC_SAMPLES; i > 0; i--) {
        ADC10CTL0 &= ~ENC;
        ADC10CTL1 = INCH_0;  // channel A0 for Vrx
        ADC10CTL0 |= ENC + ADC10SC;
        __bis_SR_register(CPUOFF + GIE);  // Enter LPM0, enable interrupts
        sum += ADC10MEM;
    }

    adcX = sum / ADC_SAMPLES;

    // Read Y-axis (Vry on P1.3)
    sum = 0;
    for (i = ADC_SAMPLES; i > 0; i--) {
        ADC10CTL0 &= ~ENC;
        ADC10CTL1 = INCH_3;  // channel A3 for Vry
        ADC10CTL0 |= ENC + ADC10SC;
        __bis_SR_register(CPUOFF + GIE);  // Enter LPM0, enable interrupts
        sum += ADC10MEM;
    }

    adcY = sum / ADC_SAMPLES;

    // Convert ADC values to voltages
    Vrx = (float)adcX / ADC_MAX_VALUE * VCC;
    Vry = (float)adcY / ADC_MAX_VALUE * VCC;

    // Calculate normalized positions (-1 to 1 range)
    posX = (Vrx - X_CENTER) / (0.5 * (X_MAX - X_MIN));
    posY = (Vry - Y_CENTER) / (0.5 * (Y_MAX - Y_MIN));

    // Calculate angle in radians
    angle = atan2(posY, posX);

    // Adjust to 0-2pi range
    if (angle < 0) {
        angle += 2 * PI;
    }

    if (abs(Vrx - X_CENTER) < 0.1 && abs(Vry - Y_CENTER) < 0.1) angle = -1;

}


//--------------------------------------------------------------------
//             Delay Microsecond Functions
//--------------------------------------------------------------------
void DelayUs(unsigned int cnt){
    unsigned char i;
    for(i=cnt ; i>0 ; i--) __no_operation();;//asm("nop"); // the command asm("nop") takes 1usec
}

//--------------------------------------------------------------------
//             Delay Millisecond Functions
//--------------------------------------------------------------------
void DelayMs(unsigned int cnt){
    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // the command asm("nop") takes roughly 1usec
}

//--------------------------------------------------------------------
//             Polling Based Delay Function
//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
//             X Milliseconds Delay
//--------------------------------------------------------------------
void msecs_delay(int X){
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

//--------------------------------------------------------------------
//             Enter LPM Mode
//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
//             Enable Interrupts
//--------------------------------------------------------------------
void enable_interrupts(){
    _BIS_SR(GIE);
}

//--------------------------------------------------------------------
//             Disable Interrupts
//--------------------------------------------------------------------
void disable_interrupts(){
    _BIC_SR(GIE);
}


//---------------------------------------------------------------------
//            TX functions
//---------------------------------------------------------------------

void send_message(){
    tx_index = 0;
    IE2 |= UCA0TXIE;               // Enable USCI_A0 TX interrupt
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (u == 1){

        UCA0TXBUF = string1[tx_index++];                 // TX next character

        if (tx_index == sizeof string1 - 1){             // TX over?
            // Disable USCI_A0 TX interrupt
            IE2 &= ~UCA0TXIE;
            u = 0;
            done_sending = 1;
        }
    }

    else {

        UCA0TXBUF = out_string[tx_index++];                 // TX next character

        if (tx_index == sizeof out_string - 1){              // TX over?
            // Disable USCI_A0 TX interrupt
            IE2 &= ~UCA0TXIE;
            done_sending = 1;


        }
    }
    if (done_sending == 1){

        done_sending = 0;
        tx_index = 0;



        // Exit from a given LPM
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
//---------------------------------------------------------------------
//            RX functions
//---------------------------------------------------------------------

void process_message(void){

    if (strcmp(in_string, "u") == 0) {
        u = 1;
        tx_index = 0;
        IE2 |= UCA0TXIE;               // Enable USCI_A0 TX interrupt
        UCA0TXBUF = string1[tx_index++];
    }
    else if (strcmp(in_string, "STATE0") == 0) state = state0;
    else if (strcmp(in_string, "STATE1") == 0) state = state1;
    else if (strcmp(in_string, "STATE2") == 0) state = state2;
    else if (strcmp(in_string, "STATE3") == 0) state = state3;
    else if (strcmp(in_string, "STATE4") == 0) state = state4;
    else if (strcmp(in_string, "1START") == 0) state1_start = 1;
    else if (strcmp(in_string, "1STOP") == 0) state1_start = 0;
    else if (state == state4){
         if (in_string[0] == '4' && in_string[1] == 'L') {
           if (in_string[3] == '1'){
               script_number = 1;
           }
           else if (in_string[3] == '2'){
               script_number = 2;
           }
           else if (in_string[3] == '3'){
               script_number = 3;
           }

           //state4_mode = 'w';
           write_script();
        }
          else if (in_string[0] == '4' && in_string[1] == 'R') {
              if (in_string[3] == '1'){
                  script_number = 1;
              }
              else if (in_string[3] == '2'){
                  script_number = 2;
              }
              else if (in_string[3] == '3'){
                  script_number = 3;
              }

//              state4_mode = 'r';
              run_script();
          }
        }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{

    if (UCA0RXBUF == '\n') {

        in_string[rx_index] = '\0';
        process_message();
        rx_index = 0;

        }
    else {
        // Add character to buffer
        in_string[rx_index++] = UCA0RXBUF;
        }


// Exit from a given LPM
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
//--------------------------------------------------------------------
//             Timer A0 Interrupt Service Routine
//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
//             ADC10 Interrupt Service Routine
//--------------------------------------------------------------------
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}

//--------------------------------------------------------------------
//             PORT2 (joystick) Interrupt Service Routine
//--------------------------------------------------------------------

#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){
    delay(debounceVal);
    JoyPressIntEn &= ~JoyPress; //disable interrupts

// selector of transition between states
    if(JoyPressIntPend & JoyPress){
        JoyPressIntPend &= ~JoyPress;

        if (state == state3){
            if (state3_start == 0){ //first press starts the spinning
                state3_start = 1;
            }
            else if (state3_start == 1){ // second press stops the spinning
                state3_start = 0;
            }
        }
        else if (state == state2){
            if (state2_mode == 'w') state2_mode = 'd';  // switch from write to delete
            else if (state2_mode == 'd') state2_mode = 'n'; // switch from delete to neutral
            else if (state2_mode == 'n') state2_mode = 'w'; // switch from neutral to write
        }


        JoyPressIntEn |= JoyPress; //enable interrupts

// Exit from a given LPM

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


