#ifndef _API_H_
#define _API_H_


void sysConfig(void);
void print2LEDs(unsigned char);
void clrLEDs(void);
void clearPWM();
void toggele();
unsigned char readSWs(void);
void incLEDs(char);
void delay(unsigned int);
void enterLPM(unsigned char);
void enable_interrupts();
void disable_interrupts();
void createPWM(int Nfreq, int uptime);

void delayInMs(int); //delay in ms
__interrupt void PBs_handler(void);

#endif







