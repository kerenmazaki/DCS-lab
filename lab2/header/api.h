#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer

// Global Variables
extern unsigned int Count;
extern unsigned int T1, T2;
extern unsigned int overflow;

void sysConfig(void);
void setByte2Port(unsigned char ch);
void clrPortByte(char ch);
void lcd_cmd(unsigned char c);
void lcd_data(unsigned char c);
void lcd_puts(const char * s);
void write_freq_tmp_LCD(void);
void write_signal_shape_tmp_LCD(void);
void lcd_init();
void lcd_strobe();
void DelayUs(unsigned int cnt);
void DelayMs(unsigned int cnt);
void delay(unsigned int t);
void enterLPM(unsigned char LPM_level);
void enable_interrupts();
void disable_interrupts();

__interrupt void PBs_handler(void);
__interrupt void PB3_handler(void);

#endif
