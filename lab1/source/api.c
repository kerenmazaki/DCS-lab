#include  "../header/api.h"
#include  "../header/hal.h" 

//#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h" //lab kit
//#else
//#include "../header/bsp_msp430x2xx.h" //pesronal kit
//#endif

int shift_val = 0x80;
int count_val;
int press_count;
int uptime;
extern int Nfreq;

int increace(int val) {
    //    LEDsArrPort *= 2;
    if (val != 0xFF) {
        val += 1;
    }
    else {
        val = 0; // restart
    }
    return val;
}

int decreace(int val) {
    //    LEDsArrPort *= 2;
    if (val != 0) {
        val -= 1;
    }
    else {
        val = 0xFF; // restart
    }
    return val;
}

void count(unsigned long int rate) {

    unsigned int i;

    if (press_count == 1) {
        for (i = 0; i < 20; i++) { // 10 seconds = 20 times, each time takes 0.5 secs
            count_val = increace(count_val);
            print2LEDs(count_val);
            delay(rate);
        }
    } 
    else {
        for (i = 0; i < 20; i++) { // 10 seconds = 20 times, each time takes 0.5 secs
            count_val = decreace(count_val);
            print2LEDs(count_val);
            delay(rate);
        }
    }
    clrLEDs();
}

int shift(int val) {
    //    LEDsArrPort *= 2;
    if (val != 0x80) {
        val = val << 1;
    }
    else {
        val = 1; // restart
    }
    return val;
}

void ShiftNum2LEDs(unsigned long int rate) {

    unsigned int i;

    for (i = 0; i < 14; i++) { // 7 seconds = 14 times, each time takes 0.5 secs
        shift_val = shift(shift_val);
        print2LEDs(shift_val);
        delay(rate);
    }
    clrLEDs();
}

void generatePwm(void) {

    clearPWM(); /// Signal starts at zero

    while (state == state3) {
        delay(delay_0);
        toggele();
        delay(delay_1);
        toggele();
    }
}

void pwm(int Nfreq) {
    // Clear PWM signal (if clearPWM() is defined elsewhere)
    clearPWM();
    // Loop while state is state4
    while (state == state4) {
        if ((SWsArrPort & 0x70) == 0x10) {// 0x01
            uptime = 0;
        }
        if ((SWsArrPort & 0x70) == 0x20){ // 0x02
            uptime = (Nfreq >> 2);
        }
        if ((SWsArrPort & 0x70) == 0x30) {// 0x03
            uptime = (Nfreq >> 1) + 5;
            }
        if ((SWsArrPort & 0x70) == 0x40) {// 0x04
            uptime = (Nfreq >> 1) + (Nfreq >> 2) + 9;
        }
        if ((SWsArrPort & 0x70) == 0x50) { // 0x05
            uptime = Nfreq;
        }

        createPWM(Nfreq, uptime);
    }
}

