#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer

void sysConfig(void);
void lcd_cmd(unsigned char c);
void lcd_data(unsigned char c);
void lcd_puts(const char * s);
void lcd_init();
void lcd_strobe();
void DelayUs(unsigned int cnt);
void DelayMs(unsigned int cnt);
void delay(unsigned int t);
void enterLPM(unsigned char LPM_level);
void enable_interrupts();
void disable_interrupts();
void Half_Sec();
void X_delay();
#endif
