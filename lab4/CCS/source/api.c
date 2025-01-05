#include  "../header/bsp.h"
#include  "../header/hal.h"
#include  "../header/api.h"
#include "string.h"
#include "stdio.h"

#define ASCII_offset 48
int count1 = 0;
int count2 = 0;
int tone_series[7] = {1000, 1250, 1500, 1750, 2000, 2250, 2500};
int X = 500;



//*********************************************************************
//            state 1 function
//*********************************************************************

void state1_func(void){
    int RGB_count = 0;
    while (state == state1){
        if (RGB_count == 0){
            RGBOut &= ~0x07;
            RGBOut |= 0x01; //Blue
            RGB_count++;
        }
        else if (RGB_count == 1){
            RGBOut &= ~0x07;
            RGBOut |= 0x02; //Green
            RGB_count++;
        }
        else if (RGB_count == 2){
            RGBOut &= ~0x07;
            RGBOut |= 0x04; // Red
            RGB_count = 0;
        }
        X_delay();
    }
}
//*********************************************************************
//            state 2 function
//*********************************************************************
void state2_func(){
    char buffer[16];
    lcd_clear();
    while (state == state2){
        sprintf(buffer, "%d", count1);
        lcd_home();
        lcd_puts(buffer);
        X_delay();
        count1 ++;
    }
}
//*********************************************************************
//            state 3 function
//*********************************************************************
void state3_func(){
    float N_out;
    float tone_f;
    const float N_SMCLK = 1048576;   // 2^20

    TA1CTL=TASSEL_2+MC_1; //SMCLK,UPMODE

    while(state==state3){
        tone_f = tone_series[count2];
        N_out = (N_SMCLK /tone_f);
        TA1CCR0 =(int)N_out;
        TA1CCR2 = (int)N_out/2;
        X_delay();
        count2 ++;
        if (count2 == 7) count2 = 0;
        }

    TA1CTL = MC_0;
}
//*********************************************************************
//            state 4 function
//*********************************************************************
void state4_func(void){
    enterLPM(lpm_mode);
}

//*********************************************************************
//            state 5 function
//*********************************************************************
void state5_func(void){
    float m = 0.00349; //3.56/1023
    float n = 0;
    float disp_val;
    char buffer[16];

    lcd_clear();
    //TA1CTL=TASSEL_2+MC_1; //SMCLK,UPMODE
    lcd_goto(5);
    lcd_puts("[V]");
    while(state==state5){
        unsigned int AdcVal=ADC10MEM; //N_ADC
        ADC10CTL0 |= ENC + ADC10SC; //to start sample
        __bis_SR_register(LPM0_bits + GIE); //sleep
        ADC10CTL0 &= ~ADC10ON; //no interrupt
        disp_val = m*AdcVal+n;

        int int_part = (int)disp_val;
        int frac_part = (int)((disp_val - int_part) * 100); // Adjust multiplier based on precision
        sprintf(buffer, "%d", int_part);
        lcd_home();
        lcd_puts(buffer);
        lcd_puts(".");
        if (frac_part < 10){
            lcd_puts("0");
        }
        sprintf(buffer, "%d", frac_part);
        lcd_puts(buffer);
        X_delay();
    }
}
//*********************************************************************
//            state 6 function
//*********************************************************************
void state6_func(void){
    lcd_clear();
    count1 = 0;
    state = state9;
}
//*********************************************************************
//            state 7 function
//*********************************************************************
void state7_func(void){
}
//*********************************************************************
//            state 8 function
//*********************************************************************
void state8_func(void){
    state = state9;
}


