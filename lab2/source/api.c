#include  "../header/bsp.h"   		// private library - API layer
#include  "../header/hal.h"   		// private library - API layer
#include  "../header/api.h"   		// private library - API layer
#include "stdio.h"


#define ASCII_offset 48


char min_tens = 0;
char min = 0;
char sec_tens = 0;
char sec = 0;

//*********************************************************************
//            count half sec
//*********************************************************************

void freq_meas(){
  float N_ADC, freq;
  unsigned int real_freq, last_freq = 0;
  float N_SMCLK = 1048576;   // 2^20
  char str_freq[] = {'\0'};
  
  lcd_clear();
  WDTCTL = WDTPW + WDTHOLD;
  write_freq_tmp_LCD(); // Write template of Frequency
 
  TA1CTL |= TASSEL_2 + MC_2 + TACLR + TAIE;         //start Timer
  
  while(state == state1){
    disable_interrupts();
    str_freq[6] = '\0';   // Reset strFreq
    T2 = T1 =  0;
    TA1CCTL2 |= CCIE;                                // enable timer interrupt
    __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0
    
    N_ADC = (T2 - T1);
    freq = 0.98*(N_SMCLK / N_ADC);       // Calculate Frequency
    real_freq = (unsigned int) freq;
    sprintf(str_freq, "%d", real_freq);
    if (((last_freq / real_freq) < 0.98) || ((last_freq / real_freq) > 1.02)){
      lcd_home();
      lcd_goto(4);
      lcd_puts(str_freq);
      if (real_freq < 10000){
        lcd_puts(" ");
      }    
      cursor_off();
      DelayMs(500);
    }
    last_freq = real_freq;
    enable_interrupts();
  }
  TA1CTL = MC_0 ; // Stop Timer
} 

//*********************************************************************
//            count half sec
//*********************************************************************
void Half_Sec(){
  TACCR0=0xFFFF;
  TA0CTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
  __bis_SR_register(LPM0_bits+GIE);
}

//*********************************************************************
//            count 0.45 sec
//*********************************************************************
void ff_mSec(){
  TACCR0=0xE661;
  TA0CTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
  __bis_SR_register(LPM0_bits+GIE);
}
//*********************************************************************
//            count_2_min
//*********************************************************************
void Counter(){
  lcd_clear();
  while (state == state2){
    lcd_home();
    lcd_putchar(min_tens + ASCII_offset);
    lcd_putchar(min + ASCII_offset);
    lcd_putchar(':');
    lcd_putchar(sec_tens + ASCII_offset);
    lcd_putchar(sec + ASCII_offset);
    
    Half_Sec();
    Half_Sec();  
    if ((SWsArrPort & 0x01) == 0x01){
      if (sec < 9){
        sec++;
      }
      else if (sec_tens < 5){
        sec = 0;
        sec_tens++;
      }
      else if (min < 9){
        sec = 0;
        sec_tens = 0;
        min ++;
      }
      else if (min_tens < 5){
        sec = 0;
        sec_tens = 0;
        min = 0; 
        min_tens ++;
      }
      else {
        sec = 0;
        sec_tens = 0;
        min = 0; 
        min_tens = 0;  
      }         
    }
  }
  lcd_clear();
}

//*********************************************************************
//            print B periodically
//*********************************************************************
void print_B(){
  int pos = -1;
  lcd_clear();
  lcd_home();
  //cursor_off();
  while (state == state4){
    pos ++;
    if (pos == 16){
    lcd_new_line();
    }
    else if (pos == 32){
      pos = 0;
      lcd_home();
    }
    lcd_putchar('B');  
    ff_mSec();
    ff_mSec();  
    lcd_cursor_left();
    lcd_puts(" ");
    //lcd_cursor_right();
  }
  lcd_clear();
}

//*********************************************************************
//            Tone Gen
//*********************************************************************
void Tone_Gen(){
    float m = 1.5;
    float n = 1010;
    float ADC_N;
    float N_SMCLK = 1048576;   // 2^20
        
  lcd_clear();
  TA1CTL=TASSEL_2+MC_1; //SMCLK,UPMODE
  while(state==state3){
    unsigned int AdcVal=ADC10MEM; //NADC
    ADC10CTL0 |= ENC + ADC10SC; //to start sample
    __bis_SR_register(LPM0_bits + GIE); //sleep
    ADC10CTL0 &= ~ADC10ON; //no interrupt
    
    ADC_N = m*AdcVal+n;
    float f_out = N_SMCLK / ADC_N;
    TA1CCR0 =(int) f_out;
    TA1CCR1 = (int) f_out/2;
  }
  TA1CTL = MC_0;
}