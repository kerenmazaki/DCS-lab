#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer

// Global Variables
extern char KB;
extern char last_KB;

void sysConfig(void);
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
void KB_Char(void);
void set_recorder_char(void);
void splitSentence(char* sentence, char words[10][11], int* numWords);
void mergeSentences(int index1, int index2);
void DMA_stop();
void Half_Sec();
void ff_mSec();
void DMA_state2(void);
void DMA_state3(void);
void DMA_state4(void);

__interrupt void PBs_handler(void);
__interrupt void PB3_handler(void);

#endif
